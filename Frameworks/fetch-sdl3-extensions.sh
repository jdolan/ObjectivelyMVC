#!/usr/bin/env bash
#
# fetch-sdl3-extensions.sh — downloads the official SDL3_image and SDL3_ttf Apple
# xcframeworks from libsdl.org on demand and caches them under Frameworks/.
#
# Core SDL3 itself comes from the ObjectivelyGPU sibling
# (../ObjectivelyGPU/Frameworks/SDL3.xcframework); this fetches only the
# extensions ObjectivelyMVC layers on top. Mirrors ObjectivelyMVC.vs15/sdl3.targets
# (Windows) and ObjectivelyGPU/Frameworks/fetch-sdl3.sh: downloaded once, on
# demand, .gitignored. Bump the versions to upgrade; delete the cached
# xcframeworks to force a re-download.
#
set -euo pipefail

SDL3_IMAGE_VERSION="${SDL3_IMAGE_VERSION:-3.4.0}"
SDL3_TTF_VERSION="${SDL3_TTF_VERSION:-3.2.2}"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Serialize concurrent invocations (parallel target builds).
LOCK="$SCRIPT_DIR/.sdl3-extensions.lock"
while ! mkdir "$LOCK" 2>/dev/null; do
    sleep 1
done
trap 'rmdir "$LOCK" 2>/dev/null || true' EXIT

fetch() {
    local repo="$1" name="$2" version="$3"
    local xcframework="$SCRIPT_DIR/$name.xcframework"

    if [ -d "$xcframework" ]; then
        echo "==> $name.xcframework: cached"
        return 0
    fi

    local url="https://github.com/libsdl-org/$repo/releases/download/release-$version/$name-$version.dmg"
    local tmp mnt
    tmp="$(mktemp -d)"
    mnt="$tmp/mnt"
    mkdir -p "$mnt"

    echo "==> Downloading $name $version"
    curl -fL --retry 3 "$url" -o "$tmp/$name.dmg"

    hdiutil attach "$tmp/$name.dmg" -nobrowse -quiet -mountpoint "$mnt"
    local src
    src="$(find "$mnt" -maxdepth 2 -name "$name.xcframework" -type d | head -1)"
    [ -n "$src" ] && cp -R "$src" "$xcframework"
    hdiutil detach "$mnt" -quiet 2>/dev/null || true
    rm -rf "$tmp"

    [ -d "$xcframework" ] || { echo "error: $name.xcframework not found in $url" >&2; return 1; }
    echo "==> $name.xcframework $version ready"
}

fetch SDL_image SDL3_image "$SDL3_IMAGE_VERSION"
fetch SDL_ttf   SDL3_ttf   "$SDL3_TTF_VERSION"
