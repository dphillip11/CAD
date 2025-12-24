#!/usr/bin/env bash
set -euo pipefail

# Convenience script to configure, build (Release) and run the native app.
BUILD_DIR=build
echo "Configuring (build dir: $BUILD_DIR)"
mkdir -p "$BUILD_DIR"
cmake -S . -B "$BUILD_DIR" -DBUILD_TESTS=OFF -D CMAKE_BUILD_TYPE=Release

echo "Building"
cmake --build "$BUILD_DIR" --config Release -- -j "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)"

# Locate the executable
EXE="$BUILD_DIR/cad_app"
if [ ! -x "$EXE" ]; then
  if [ -x "$BUILD_DIR/Release/cad_app" ]; then
    EXE="$BUILD_DIR/Release/cad_app"
  fi
fi

if [ ! -x "$EXE" ]; then
  echo "Error: built executable not found at $EXE"
  exit 1
fi

echo "Running $EXE"
"$EXE"
