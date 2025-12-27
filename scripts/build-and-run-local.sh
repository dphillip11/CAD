#!/usr/bin/env bash
set -euo pipefail

# Convenience script to configure, build (Release) and run the native app.
# Usage: ./build-and-run-local.sh [debug|release]
# Default: release

BUILD_TYPE="${1:-release}"
BUILD_DIR="build"

case "$BUILD_TYPE" in
    debug|Debug|DEBUG)
        BUILD_TYPE="Debug"
        ;;
    release|Release|RELEASE)
        BUILD_TYPE="Release"
        ;;
    *)
        echo "Invalid build type: $BUILD_TYPE. Use 'debug' or 'release'."
        exit 1
        ;;
esac

echo "Configuring (build dir: $BUILD_DIR, type: $BUILD_TYPE)"
# Clean build directory to avoid stale builds
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi
mkdir -p "$BUILD_DIR"

cmake -S . -B "$BUILD_DIR" -DBUILD_TESTS=OFF -D CMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "Building ($BUILD_TYPE)"
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -- -j "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)"

# Locate the executable
EXE="$BUILD_DIR/CAD"
if [ ! -x "$EXE" ]; then
    EXE="$BUILD_DIR/$BUILD_TYPE/CAD"
    if [ ! -x "$EXE" ]; then
        echo "Error: built executable not found at $EXE"
        exit 1
    fi
fi

echo "Running $EXE ($BUILD_TYPE build)"
"$EXE"
