#!/usr/bin/env bash
#
# build-xcframework.sh — builds ObjectivelyMVC.xcframework for iOS device + simulator.
#
# Also builds/downloads all dependencies:
#   Frameworks/libcurl.xcframework        (via Objectively build script)
#   Frameworks/Objectively.xcframework   (via Objectively build script)
#   Frameworks/SDL3.xcframework          (via SDL3 Xcode project)
#   Frameworks/SDL3_image.xcframework    (via SDL3_image Xcode project)
#   Frameworks/SDL3_ttf.xcframework      (via SDL3_ttf Xcode project)
#   Frameworks/ObjectivelyMVC.xcframework
#
# Usage:
#   scripts/build-xcframework.sh
#
# Optional env vars:
#   SDL3_VERSION        (default: release-3.4.10)
#   SDL3_IMAGE_VERSION  (default: release-3.4.0)
#   SDL3_TTF_VERSION    (default: release-3.2.2)
#   OBJECTIVELY_BRANCH  (default: main)
#   IOS_MIN             (default: 14.0)
#
# NOTE: This script runs `make distclean` in the source directory to allow
# out-of-tree cross-compilation. Re-run `./configure && make` in the repo
# root to restore your development build afterwards.
#
set -e -o pipefail

# Xcode runs scripts with a stripped PATH. Restore Homebrew and common tool paths.
export PATH="/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MVC_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$MVC_DIR/.build-xcframework"
FRAMEWORKS_DIR="$MVC_DIR/Frameworks"

SDL3_VERSION="${SDL3_VERSION:-release-3.4.10}"
SDL3_IMAGE_VERSION="${SDL3_IMAGE_VERSION:-release-3.4.0}"
SDL3_TTF_VERSION="${SDL3_TTF_VERSION:-release-3.2.2}"
OBJECTIVELY_BRANCH="${OBJECTIVELY_BRANCH:-main}"
IOS_MIN="${IOS_MIN:-14.0}"
MACOS_MIN="${MACOS_MIN:-12.0}"

MACOSX_SDK=$(xcrun --sdk macosx --show-sdk-path)
IPHONEOS_SDK=$(xcrun --sdk iphoneos --show-sdk-path)
IPHONESIMULATOR_SDK=$(xcrun --sdk iphonesimulator --show-sdk-path)
CLANG=$(xcrun --find clang)
LIPO=$(xcrun --find lipo)
NPROC=$(sysctl -n hw.logicalcpu)

mkdir -p "$BUILD_DIR" "$FRAMEWORKS_DIR"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

clone_sdl() {
    local name="$1"
    local repo="$2"
    local tag="$3"
    local dir="$BUILD_DIR/$name"
    if [ ! -d "$dir/.git" ]; then
        echo "==> Cloning $name $tag"
        git clone --depth 1 --branch "$tag" "$repo" "$dir" -q
    fi
}

build_sdl_cmake_xcframework() {
    local name="$1"         # e.g. SDL3
    local framework="$2"    # framework binary name, e.g. SDL3
    local extra_cmake="$3"  # extra -D flags (optional; word-split, so no spaces in values)
    local sdl3_base="$4"    # base install dir for SDL3 (SDL3_image/ttf only), optional
    local init_cache="$5"   # optional path to a cmake cache init file (-C), for flags with spaces
    local output="$FRAMEWORKS_DIR/$name.xcframework"

    if [ -d "$output" ]; then
        echo "==> $name.xcframework: cached"
        return 0
    fi

    local srcdir="$BUILD_DIR/$name"
    local prefix_device="$BUILD_DIR/$name-install-iphoneos"
    local prefix_sim="$BUILD_DIR/$name-install-iphonesimulator"
    local prefix_macos="$BUILD_DIR/$name-install-macos"

    for slice in device simulator macos; do
        local sdk arch sysroot depl_target cmake_sys min_ver_key prefix build_dir sdl3_dir
        if [ "$slice" = "device" ]; then
            sdk="$IPHONEOS_SDK"
            arch="arm64"
            sysroot="iphoneos"
            depl_target="$IOS_MIN"
            cmake_sys="-DCMAKE_SYSTEM_NAME=iOS"
            min_ver_key="MinimumOSVersion"
            prefix="$prefix_device"
            [ -n "$sdl3_base" ] && sdl3_dir="-DSDL3_DIR=${sdl3_base}-iphoneos/lib/cmake/SDL3"
        elif [ "$slice" = "simulator" ]; then
            sdk="$IPHONESIMULATOR_SDK"
            arch="arm64;x86_64"
            sysroot="iphonesimulator"
            depl_target="$IOS_MIN"
            cmake_sys="-DCMAKE_SYSTEM_NAME=iOS"
            min_ver_key="MinimumOSVersion"
            prefix="$prefix_sim"
            [ -n "$sdl3_base" ] && sdl3_dir="-DSDL3_DIR=${sdl3_base}-iphonesimulator/lib/cmake/SDL3"
        else  # macos
            sdk="$MACOSX_SDK"
            arch="arm64;x86_64"
            sysroot="macos"
            depl_target="$MACOS_MIN"
            cmake_sys=""  # no CMAKE_SYSTEM_NAME — cmake defaults to Darwin (macOS)
            min_ver_key="LSMinimumSystemVersion"
            prefix="$prefix_macos"
            [ -n "$sdl3_base" ] && sdl3_dir="-DSDL3_DIR=${sdl3_base}-macos/lib/cmake/SDL3"
        fi
        build_dir="$BUILD_DIR/$name-build-$sysroot"

        [ -d "$prefix/$framework.framework" ] && continue

        echo "==> $name ($sysroot): configuring"
        mkdir -p "$build_dir"
        cmake -S "$srcdir" -B "$build_dir" \
            ${cmake_sys:-} \
            -DCMAKE_OSX_ARCHITECTURES="$arch" \
            -DCMAKE_OSX_SYSROOT="$sdk" \
            -DCMAKE_OSX_DEPLOYMENT_TARGET="$depl_target" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="$prefix" \
            -DBUILD_SHARED_LIBS=ON \
            ${sdl3_dir:-} \
            ${init_cache:+-C "$init_cache"} \
            $extra_cmake \
            > "$build_dir/cmake.log" 2>&1

        echo "==> $name ($sysroot): building"
        cmake --build "$build_dir" --target install -- -j"$NPROC" \
            >> "$build_dir/cmake.log" 2>&1

        # Bundle the dylib into a .framework
        echo "==> $name ($sysroot): bundling framework"
        local dylib
        dylib=$(find "$prefix/lib" -name "lib${framework}.dylib" ! -type l 2>/dev/null | head -1)
        [ -z "$dylib" ] && dylib=$(find "$prefix/lib" -name "lib${framework}.*.dylib" ! -type l 2>/dev/null | head -1)

        local fwdir="$prefix/$framework.framework"
        mkdir -p "$fwdir/Headers/SDL3"

        cp "$dylib" "$fwdir/$framework"
        xcrun install_name_tool -id "@rpath/$framework.framework/$framework" "$fwdir/$framework"

        # Fix SDL3 dependency path for SDL3_image / SDL3_ttf
        local old_sdl3
        old_sdl3=$(xcrun otool -L "$fwdir/$framework" | awk '/libSDL3[^_]/{print $1}' | head -1)
        if [ -n "$old_sdl3" ]; then
            xcrun install_name_tool -change "$old_sdl3" "@rpath/SDL3.framework/SDL3" "$fwdir/$framework"
        fi

        # Install headers (SDL3-family headers all live under include/SDL3/)
        [ -d "$prefix/include/SDL3" ] && cp "$prefix/include/SDL3/"*.h "$fwdir/Headers/SDL3/"

        local bundle_id="${name//_/-}"  # underscores invalid in bundle IDs: SDL3_ttf → SDL3-ttf
        cat > "$fwdir/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key><string>en</string>
    <key>CFBundleExecutable</key><string>$framework</string>
    <key>CFBundleIdentifier</key><string>org.libsdl.$bundle_id</string>
    <key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
    <key>CFBundleName</key><string>$framework</string>
    <key>CFBundlePackageType</key><string>FMWK</string>
    <key>CFBundleShortVersionString</key><string>1.0</string>
    <key>CFBundleVersion</key><string>1</string>
    <key>$min_ver_key</key><string>$depl_target</string>
</dict>
</plist>
EOF
    done

    echo "==> Creating $name.xcframework"
    rm -rf "$output"
    xcodebuild -create-xcframework \
        -framework "$prefix_device/$framework.framework" \
        -framework "$prefix_sim/$framework.framework" \
        -framework "$prefix_macos/$framework.framework" \
        -output "$output"
}

# ---------------------------------------------------------------------------
# SDL3
# ---------------------------------------------------------------------------

clone_sdl SDL3 https://github.com/libsdl-org/SDL.git "$SDL3_VERSION"
build_sdl_cmake_xcframework SDL3 SDL3 \
    "-DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF"

# ---------------------------------------------------------------------------
# SDL3_image
# ---------------------------------------------------------------------------

clone_sdl SDL3_image https://github.com/libsdl-org/SDL_image.git "$SDL3_IMAGE_VERSION"
build_sdl_cmake_xcframework SDL3_image SDL3_image \
    "-DSDLIMAGE_SAMPLES=OFF -DSDLIMAGE_TESTS=OFF" \
    "$BUILD_DIR/SDL3-install"

# ---------------------------------------------------------------------------
# SDL3_ttf
# ---------------------------------------------------------------------------

clone_sdl SDL3_ttf https://github.com/libsdl-org/SDL_ttf.git "$SDL3_TTF_VERSION"
# SDL3_ttf vendored deps (FreeType, HarfBuzz) are git submodules
if [ ! -f "$BUILD_DIR/SDL3_ttf/external/harfbuzz/CMakeLists.txt" ]; then
    echo "==> SDL3_ttf: downloading vendored deps"
    (cd "$BUILD_DIR/SDL3_ttf" && bash external/download.sh --depth 1) > /dev/null 2>&1
fi
# SDL3_ttf's vendored HarfBuzz uses CoreText shaper on Apple platforms;
# when building as a shared lib those framework symbols must be linked explicitly.
SDL3_TTF_INIT_CACHE="$BUILD_DIR/sdl3_ttf_init.cmake"
cat > "$SDL3_TTF_INIT_CACHE" << 'EOF'
set(CMAKE_SHARED_LINKER_FLAGS "-framework CoreFoundation -framework CoreText -framework CoreGraphics" CACHE STRING "")
EOF

build_sdl_cmake_xcframework SDL3_ttf SDL3_ttf \
    "-DSDLTTF_VENDORED=ON -DSDLTTF_SAMPLES=OFF -DSDLTTF_TESTS=OFF" \
    "$BUILD_DIR/SDL3-install" \
    "$SDL3_TTF_INIT_CACHE"

# ---------------------------------------------------------------------------
# Objectively + libcurl (build from source)
# ---------------------------------------------------------------------------

# Auto-detect sibling Objectively checkout; fall back to GitHub clone.
if [ -z "$OBJECTIVELY_DIR" ]; then
    _sibling="$(cd "$MVC_DIR/.." && pwd)/Objectively"
    [ -d "$_sibling/.git" ] && OBJECTIVELY_DIR="$_sibling"
fi

if [ -n "$OBJECTIVELY_DIR" ] && [ -d "$OBJECTIVELY_DIR/.git" ]; then
    OBJ_DIR="$OBJECTIVELY_DIR"
    echo "==> Using local Objectively at $OBJ_DIR"
    # Build sibling xcframeworks if not yet built, then always sync to our Frameworks/
    # (ensures we pick up macOS slices from the local, possibly unpushed, branch)
    if [ ! -d "$OBJ_DIR/Frameworks/Objectively.xcframework" ] || \
       [ ! -d "$OBJ_DIR/Frameworks/libcurl.xcframework" ]; then
        echo "==> Building Objectively xcframeworks (sibling)"
        bash "$OBJ_DIR/scripts/build-xcframework.sh"
    fi
    echo "==> Copying Objectively xcframeworks from sibling"
    cp -r "$OBJ_DIR/Frameworks/"*.xcframework "$FRAMEWORKS_DIR/"
else
    OBJ_DIR="$BUILD_DIR/Objectively"
    if [ ! -d "$OBJ_DIR/.git" ]; then
        echo "==> Cloning Objectively ($OBJECTIVELY_BRANCH)"
        git clone --depth 1 --branch "$OBJECTIVELY_BRANCH" \
            https://github.com/jdolan/Objectively.git "$OBJ_DIR" -q
    else
        echo "==> Updating Objectively ($OBJECTIVELY_BRANCH)"
        git -C "$OBJ_DIR" fetch origin "$OBJECTIVELY_BRANCH" -q
        git -C "$OBJ_DIR" reset --hard "origin/$OBJECTIVELY_BRANCH" -q
    fi
    if [ ! -d "$FRAMEWORKS_DIR/Objectively.xcframework" ] || \
       [ ! -d "$FRAMEWORKS_DIR/libcurl.xcframework" ]; then
        echo "==> Building Objectively xcframeworks"
        bash "$OBJ_DIR/scripts/build-xcframework.sh"
        cp -r "$OBJ_DIR/Frameworks/"*.xcframework "$FRAMEWORKS_DIR/"
    else
        echo "==> Objectively.xcframework: cached"
        echo "==> libcurl.xcframework: cached"
    fi
fi

# ---------------------------------------------------------------------------
# ObjectivelyMVC
# ---------------------------------------------------------------------------

if [ -d "$FRAMEWORKS_DIR/ObjectivelyMVC.xcframework" ]; then
    echo "==> ObjectivelyMVC.xcframework: cached"
else

# Include paths for MVC compilation — headers are arch-independent; use ios-arm64 slice
SDL3_HEADERS="$BUILD_DIR/SDL3-install-iphoneos/include"
SDL3_IMAGE_HEADERS="$BUILD_DIR/SDL3_image-install-iphoneos/include"
SDL3_TTF_HEADERS="$BUILD_DIR/SDL3_ttf-install-iphoneos/include"
OBJ_HEADERS="$FRAMEWORKS_DIR/Objectively.xcframework/ios-arm64/Objectively.framework/Headers"

echo "==> Preparing ObjectivelyMVC autotools"
pushd "$MVC_DIR" > /dev/null
make distclean > /dev/null 2>&1 || true
autoreconf -i > /dev/null 2>&1
popd > /dev/null

build_mvc_slice() {
    local name="$1"
    local sdk="$2"
    local arch="$3"
    local min_flag="$4"
    local host="$5"
    local obj_fw="$6"   # path to matching Objectively.framework slice

    local prefix="$BUILD_DIR/mvc-install-$name"
    local builddir="$BUILD_DIR/mvc-build-$name"
    local sysroot="${name%-*}"  # iphoneos-arm64 → iphoneos, macos-arm64 → macos

    # Platform-specific flags
    local gl_framework min_ver_key min_ver_val
    if [ "$sysroot" = "macos" ]; then
        gl_framework="-framework OpenGL"
        min_ver_key="LSMinimumSystemVersion"
        min_ver_val="$MACOS_MIN"
    else
        gl_framework="-framework OpenGLES"
        min_ver_key="MinimumOSVersion"
        min_ver_val="$IOS_MIN"
    fi

    if [ -f "$prefix/ObjectivelyMVC.framework/ObjectivelyMVC" ] && \
       [ "$(wc -c < "$prefix/ObjectivelyMVC.framework/ObjectivelyMVC")" -gt 1000 ]; then
        echo "==> ObjectivelyMVC $name: cached"
        return 0
    fi

    local include_flags="\
        -I$SDL3_HEADERS \
        -I$SDL3_IMAGE_HEADERS \
        -I$SDL3_TTF_HEADERS \
        -I$OBJ_HEADERS"

    echo "==> ObjectivelyMVC $name: configuring"
    mkdir -p "$builddir"
    pushd "$builddir" > /dev/null

    SDL3_CFLAGS="-I$SDL3_HEADERS" \
    SDL3_LIBS="" \
    SDL3_image_CFLAGS="-I$SDL3_IMAGE_HEADERS" \
    SDL3_image_LIBS="" \
    SDL3_ttf_CFLAGS="-I$SDL3_TTF_HEADERS" \
    SDL3_ttf_LIBS="" \
    OBJECTIVELY_CFLAGS="-I$OBJ_HEADERS" \
    OBJECTIVELY_LIBS="" \
    CHECK_CFLAGS=" " CHECK_LIBS=" " \
    "$MVC_DIR/configure" \
        --host="$host" \
        --prefix="$prefix" \
        --disable-shared --enable-static \
        CC="$CLANG" \
        CPPFLAGS="-arch $arch -isysroot $sdk $min_flag $include_flags" \
        CFLAGS="-arch $arch -isysroot $sdk $min_flag $include_flags" \
        LDFLAGS="-arch $arch -isysroot $sdk $min_flag" \
        > "$builddir/configure.log" 2>&1 || (cat "$builddir/configure.log" >&2; exit 1)

    echo "==> ObjectivelyMVC $name: building"
    make -j"$NPROC" -C Sources >> "$builddir/make.log" 2>&1

    echo "==> ObjectivelyMVC $name: linking framework"
    local objdir="$builddir/Sources/ObjectivelyMVC"
    local fwdir="$prefix/ObjectivelyMVC.framework"

    mkdir -p "$fwdir/Headers/ObjectivelyMVC"

    "$CLANG" -arch "$arch" -isysroot "$sdk" $min_flag \
        -dynamiclib \
        -Wl,-install_name,@rpath/ObjectivelyMVC.framework/ObjectivelyMVC \
        -F"$(dirname "$obj_fw")" -framework Objectively \
        -F"$BUILD_DIR/SDL3-install-$sysroot" -framework SDL3 \
        -F"$BUILD_DIR/SDL3_image-install-$sysroot" -framework SDL3_image \
        -F"$BUILD_DIR/SDL3_ttf-install-$sysroot" -framework SDL3_ttf \
        $gl_framework \
        "$objdir"/*.o \
        -o "$fwdir/ObjectivelyMVC"

    cp "$MVC_DIR/Sources/ObjectivelyMVC/"*.h "$fwdir/Headers/ObjectivelyMVC/"
    cp "$MVC_DIR/Sources/ObjectivelyMVC.h" "$fwdir/Headers/"

    cat > "$fwdir/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key><string>en</string>
    <key>CFBundleExecutable</key><string>ObjectivelyMVC</string>
    <key>CFBundleIdentifier</key><string>com.jaydolan.ObjectivelyMVC</string>
    <key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
    <key>CFBundleName</key><string>ObjectivelyMVC</string>
    <key>CFBundlePackageType</key><string>FMWK</string>
    <key>CFBundleShortVersionString</key><string>1.0</string>
    <key>CFBundleVersion</key><string>1</string>
    <key>$min_ver_key</key><string>$min_ver_val</string>
</dict>
</plist>
EOF

    popd > /dev/null
}

build_mvc_slice "iphoneos-arm64" \
    "$IPHONEOS_SDK" "arm64" "-miphoneos-version-min=$IOS_MIN" \
    "arm-apple-darwin" \
    "$OBJ_DIR/.build-xcframework/objectively-install-iphoneos-arm64/Objectively.framework"

build_mvc_slice "iphonesimulator-arm64" \
    "$IPHONESIMULATOR_SDK" "arm64" "-mios-simulator-version-min=$IOS_MIN" \
    "arm-apple-darwin" \
    "$OBJ_DIR/.build-xcframework/objectively-install-iphonesimulator-arm64/Objectively.framework"

build_mvc_slice "iphonesimulator-x86_64" \
    "$IPHONESIMULATOR_SDK" "x86_64" "-mios-simulator-version-min=$IOS_MIN" \
    "x86_64-apple-darwin" \
    "$OBJ_DIR/.build-xcframework/objectively-install-iphonesimulator-x86_64/Objectively.framework"

echo "==> ObjectivelyMVC simulator: lipo arm64 + x86_64"
MVC_SIM_DIR="$BUILD_DIR/mvc-install-iphonesimulator-fat"
mkdir -p "$MVC_SIM_DIR/ObjectivelyMVC.framework/Headers"
"$LIPO" -create \
    "$BUILD_DIR/mvc-install-iphonesimulator-arm64/ObjectivelyMVC.framework/ObjectivelyMVC" \
    "$BUILD_DIR/mvc-install-iphonesimulator-x86_64/ObjectivelyMVC.framework/ObjectivelyMVC" \
    -output "$MVC_SIM_DIR/ObjectivelyMVC.framework/ObjectivelyMVC"
cp -r "$BUILD_DIR/mvc-install-iphoneos-arm64/ObjectivelyMVC.framework/Headers/." \
    "$MVC_SIM_DIR/ObjectivelyMVC.framework/Headers/"
cp "$BUILD_DIR/mvc-install-iphoneos-arm64/ObjectivelyMVC.framework/Info.plist" \
    "$MVC_SIM_DIR/ObjectivelyMVC.framework/"

build_mvc_slice "macos-arm64" \
    "$MACOSX_SDK" "arm64" "-mmacosx-version-min=$MACOS_MIN" \
    "aarch64-apple-darwin" \
    "$OBJ_DIR/.build-xcframework/objectively-install-macos-arm64/Objectively.framework"

build_mvc_slice "macos-x86_64" \
    "$MACOSX_SDK" "x86_64" "-mmacosx-version-min=$MACOS_MIN" \
    "x86_64-apple-darwin" \
    "$OBJ_DIR/.build-xcframework/objectively-install-macos-x86_64/Objectively.framework"

echo "==> ObjectivelyMVC macOS: lipo arm64 + x86_64"
MVC_MACOS_DIR="$BUILD_DIR/mvc-install-macos-fat"
mkdir -p "$MVC_MACOS_DIR/ObjectivelyMVC.framework/Headers"
"$LIPO" -create \
    "$BUILD_DIR/mvc-install-macos-arm64/ObjectivelyMVC.framework/ObjectivelyMVC" \
    "$BUILD_DIR/mvc-install-macos-x86_64/ObjectivelyMVC.framework/ObjectivelyMVC" \
    -output "$MVC_MACOS_DIR/ObjectivelyMVC.framework/ObjectivelyMVC"
cp -r "$BUILD_DIR/mvc-install-iphoneos-arm64/ObjectivelyMVC.framework/Headers/." \
    "$MVC_MACOS_DIR/ObjectivelyMVC.framework/Headers/"
cp "$BUILD_DIR/mvc-install-macos-arm64/ObjectivelyMVC.framework/Info.plist" \
    "$MVC_MACOS_DIR/ObjectivelyMVC.framework/"

echo "==> Creating ObjectivelyMVC.xcframework"
rm -rf "$FRAMEWORKS_DIR/ObjectivelyMVC.xcframework"
xcodebuild -create-xcframework \
    -framework "$BUILD_DIR/mvc-install-iphoneos-arm64/ObjectivelyMVC.framework" \
    -framework "$MVC_SIM_DIR/ObjectivelyMVC.framework" \
    -framework "$MVC_MACOS_DIR/ObjectivelyMVC.framework" \
    -output "$FRAMEWORKS_DIR/ObjectivelyMVC.xcframework"

fi # ObjectivelyMVC.xcframework cache check

echo ""
echo "Done! All xcframeworks contain: macos-arm64_x86_64 + ios-arm64 + ios-arm64_x86_64-simulator"
echo "  $FRAMEWORKS_DIR/SDL3.xcframework"
echo "  $FRAMEWORKS_DIR/SDL3_image.xcframework"
echo "  $FRAMEWORKS_DIR/SDL3_ttf.xcframework"
echo "  $FRAMEWORKS_DIR/libcurl.xcframework"
echo "  $FRAMEWORKS_DIR/Objectively.xcframework"
echo "  $FRAMEWORKS_DIR/ObjectivelyMVC.xcframework"
