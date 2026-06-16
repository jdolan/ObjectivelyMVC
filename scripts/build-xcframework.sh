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

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MVC_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$MVC_DIR/.build-xcframework"
FRAMEWORKS_DIR="$MVC_DIR/Frameworks"

SDL3_VERSION="${SDL3_VERSION:-release-3.4.10}"
SDL3_IMAGE_VERSION="${SDL3_IMAGE_VERSION:-release-3.4.0}"
SDL3_TTF_VERSION="${SDL3_TTF_VERSION:-release-3.2.2}"
OBJECTIVELY_BRANCH="${OBJECTIVELY_BRANCH:-feature/ios}"
IOS_MIN="${IOS_MIN:-14.0}"

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
    local lib="$2"          # e.g. libSDL3.a
    local extra_cmake="$3"  # extra -D flags (optional, no SDL3_DIR — set per slice)
    local sdl3_base="$4"    # base install dir for SDL3 (e.g. $BUILD_DIR/SDL3-install), optional
    local output="$FRAMEWORKS_DIR/$name.xcframework"

    if [ -d "$output" ]; then
        echo "==> $name.xcframework: cached"
        return 0
    fi

    local srcdir="$BUILD_DIR/$name"
    local prefix_device="$BUILD_DIR/$name-install-iphoneos"
    local prefix_sim="$BUILD_DIR/$name-install-iphonesimulator"

    for slice in device simulator; do
        local sdk arch sysroot min_flag prefix build_dir sdl3_dir
        if [ "$slice" = "device" ]; then
            sdk="$IPHONEOS_SDK"
            arch="arm64"
            sysroot="iphoneos"
            min_flag="-miphoneos-version-min=$IOS_MIN"
            prefix="$prefix_device"
            [ -n "$sdl3_base" ] && sdl3_dir="-DSDL3_DIR=${sdl3_base}-iphoneos/lib/cmake/SDL3"
        else
            sdk="$IPHONESIMULATOR_SDK"
            arch="arm64;x86_64"
            sysroot="iphonesimulator"
            min_flag="-mios-simulator-version-min=$IOS_MIN"
            prefix="$prefix_sim"
            [ -n "$sdl3_base" ] && sdl3_dir="-DSDL3_DIR=${sdl3_base}-iphonesimulator/lib/cmake/SDL3"
        fi
        build_dir="$BUILD_DIR/$name-build-$sysroot"

        [ -f "$prefix/lib/$lib" ] && continue

        echo "==> $name ($sysroot): configuring"
        mkdir -p "$build_dir"
        cmake -S "$srcdir" -B "$build_dir" \
            -DCMAKE_SYSTEM_NAME=iOS \
            -DCMAKE_OSX_ARCHITECTURES="$arch" \
            -DCMAKE_OSX_SYSROOT="$sdk" \
            -DCMAKE_OSX_DEPLOYMENT_TARGET="$IOS_MIN" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="$prefix" \
            -DBUILD_SHARED_LIBS=OFF \
            ${sdl3_dir:-} \
            $extra_cmake \
            > "$build_dir/cmake.log" 2>&1

        echo "==> $name ($sysroot): building"
        cmake --build "$build_dir" --target install -- -j"$NPROC" \
            >> "$build_dir/cmake.log" 2>&1
    done

    echo "==> Creating $name.xcframework"
    rm -rf "$output"
    xcodebuild -create-xcframework \
        -library "$prefix_device/lib/$lib" \
        -headers "$prefix_device/include" \
        -library "$prefix_sim/lib/$lib" \
        -headers "$prefix_sim/include" \
        -output "$output"
}

# ---------------------------------------------------------------------------
# SDL3
# ---------------------------------------------------------------------------

clone_sdl SDL3 https://github.com/libsdl-org/SDL.git "$SDL3_VERSION"
build_sdl_cmake_xcframework SDL3 libSDL3.a \
    "-DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF"

# ---------------------------------------------------------------------------
# SDL3_image
# ---------------------------------------------------------------------------

clone_sdl SDL3_image https://github.com/libsdl-org/SDL_image.git "$SDL3_IMAGE_VERSION"
build_sdl_cmake_xcframework SDL3_image libSDL3_image.a \
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
build_sdl_cmake_xcframework SDL3_ttf libSDL3_ttf.a \
    "-DSDLTTF_VENDORED=ON -DSDLTTF_SAMPLES=OFF -DSDLTTF_TESTS=OFF" \
    "$BUILD_DIR/SDL3-install"

# ---------------------------------------------------------------------------
# Objectively + libcurl (build from source)
# ---------------------------------------------------------------------------

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

# ---------------------------------------------------------------------------
# ObjectivelyMVC
# ---------------------------------------------------------------------------

# Headers exposed by each xcframework slice (device arm64 path)
# CMake-built xcframeworks use a flat Headers/ dir with per-library subdirectories
# (e.g. Headers/SDL3/SDL_assert.h), so -I must point to Headers/, not Headers/SDL3/.
SDL3_HEADERS="$FRAMEWORKS_DIR/SDL3.xcframework/ios-arm64/Headers"
SDL3_IMAGE_HEADERS="$FRAMEWORKS_DIR/SDL3_image.xcframework/ios-arm64/Headers"
SDL3_TTF_HEADERS="$FRAMEWORKS_DIR/SDL3_ttf.xcframework/ios-arm64/Headers"
OBJ_HEADERS="$FRAMEWORKS_DIR/Objectively.xcframework/ios-arm64/Headers"

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

    local prefix="$BUILD_DIR/mvc-install-$name"
    local builddir="$BUILD_DIR/mvc-build-$name"

    if [ -f "$prefix/lib/libObjectivelyMVC.a" ] && \
       [ "$(wc -c < "$prefix/lib/libObjectivelyMVC.a")" -gt 1000 ]; then
        echo "==> ObjectivelyMVC $name: cached"
        return 0
    fi

    local curl_prefix
    if [[ "$name" == *"iphonesimulator"* ]]; then
        # For the simulator fat lipo step we just need any slice's headers;
        # the actual .a is built per-arch. Use the matching curl slice.
        curl_prefix="$BUILD_DIR/curl-install-$name"
        [ -d "$curl_prefix" ] || curl_prefix="$(find "$OBJ_DIR/.build-xcframework" -maxdepth 1 -name "curl-install-$name" | head -1)"
    else
        curl_prefix="$(find "$OBJ_DIR/.build-xcframework" -maxdepth 1 -name "curl-install-$name" | head -1)"
    fi
    [ -z "$curl_prefix" ] && curl_prefix="$OBJ_DIR/.build-xcframework/curl-install-$name"

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
    CHECK_CFLAGS="" CHECK_LIBS="" \
    "$MVC_DIR/configure" \
        --host="$host" \
        --prefix="$prefix" \
        --disable-shared --enable-static \
        CC="$CLANG" \
        CPPFLAGS="-arch $arch -isysroot $sdk $min_flag $include_flags" \
        CFLAGS="-arch $arch -isysroot $sdk $min_flag $include_flags" \
        LDFLAGS="-arch $arch -isysroot $sdk $min_flag" \
        > "$builddir/configure.log" 2>&1

    echo "==> ObjectivelyMVC $name: building"
    make -j"$NPROC" -C Sources >> "$builddir/make.log" 2>&1

    echo "==> ObjectivelyMVC $name: archiving"
    local objdir="$builddir/Sources/ObjectivelyMVC"
    mkdir -p "$prefix/lib" "$prefix/include/ObjectivelyMVC"

    ar cr "$prefix/lib/libObjectivelyMVC.a" "$objdir"/*.o
    ranlib "$prefix/lib/libObjectivelyMVC.a"

    cp "$MVC_DIR/Sources/ObjectivelyMVC/"*.h "$prefix/include/ObjectivelyMVC/"
    # Include umbrella header one level up
    cp "$MVC_DIR/Sources/ObjectivelyMVC.h" "$prefix/include/"

    popd > /dev/null
}

build_mvc_slice "iphoneos-arm64" \
    "$IPHONEOS_SDK" "arm64" "-miphoneos-version-min=$IOS_MIN" \
    "arm-apple-darwin"

build_mvc_slice "iphonesimulator-arm64" \
    "$IPHONESIMULATOR_SDK" "arm64" "-mios-simulator-version-min=$IOS_MIN" \
    "arm-apple-darwin"

build_mvc_slice "iphonesimulator-x86_64" \
    "$IPHONESIMULATOR_SDK" "x86_64" "-mios-simulator-version-min=$IOS_MIN" \
    "x86_64-apple-darwin"

echo "==> ObjectivelyMVC simulator: lipo arm64 + x86_64"
MVC_SIM_DIR="$BUILD_DIR/mvc-install-iphonesimulator-fat"
mkdir -p "$MVC_SIM_DIR/lib"
"$LIPO" -create \
    "$BUILD_DIR/mvc-install-iphonesimulator-arm64/lib/libObjectivelyMVC.a" \
    "$BUILD_DIR/mvc-install-iphonesimulator-x86_64/lib/libObjectivelyMVC.a" \
    -output "$MVC_SIM_DIR/lib/libObjectivelyMVC.a"
cp -r "$BUILD_DIR/mvc-install-iphoneos-arm64/include" "$MVC_SIM_DIR/"

echo "==> Creating ObjectivelyMVC.xcframework"
rm -rf "$FRAMEWORKS_DIR/ObjectivelyMVC.xcframework"
xcodebuild -create-xcframework \
    -library "$BUILD_DIR/mvc-install-iphoneos-arm64/lib/libObjectivelyMVC.a" \
    -headers "$BUILD_DIR/mvc-install-iphoneos-arm64/include" \
    -library "$MVC_SIM_DIR/lib/libObjectivelyMVC.a" \
    -headers "$MVC_SIM_DIR/include" \
    -output "$FRAMEWORKS_DIR/ObjectivelyMVC.xcframework"

echo ""
echo "Done!"
echo "  $FRAMEWORKS_DIR/SDL3.xcframework"
echo "  $FRAMEWORKS_DIR/SDL3_image.xcframework"
echo "  $FRAMEWORKS_DIR/SDL3_ttf.xcframework"
echo "  $FRAMEWORKS_DIR/libcurl.xcframework"
echo "  $FRAMEWORKS_DIR/Objectively.xcframework"
echo "  $FRAMEWORKS_DIR/ObjectivelyMVC.xcframework"
