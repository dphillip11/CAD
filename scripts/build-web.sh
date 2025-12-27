#!/usr/bin/env bash
set -euo pipefail

# Build a browser-ready HTML + WASM file using Emscripten and CMake.
# Usage: source /path/to/emsdk/emsdk_env.sh; ./scripts/build-web.sh

if ! command -v emcc >/dev/null 2>&1; then
  echo "emcc not found. Install and activate Emscripten (see https://emscripten.org/docs/getting_started/downloads.html)"
  echo "Make sure to run: source /path/to/emsdk/emsdk_env.sh"
  exit 1
fi

if ! command -v emcmake >/dev/null 2>&1; then
  echo "emcmake not found. Make sure Emscripten is properly installed and activated."
  exit 1
fi

BUILD_DIR=web-build
echo "Configuring Emscripten build (build dir: $BUILD_DIR)"
mkdir -p "$BUILD_DIR"

# Use direct cmake with Emscripten toolchain
cmake -S . -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE=/opt/homebrew/Cellar/emscripten/4.0.7/libexec/cmake/Modules/Platform/Emscripten.cmake -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release

echo "Building with Emscripten"
cmake --build "$BUILD_DIR" --config Release -- -j "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)"

# Copy the built files to web directory
mkdir -p web
if [ -f "$BUILD_DIR/CAD.html" ]; then
  cp "$BUILD_DIR/CAD.html" web/hello.html
  cp "$BUILD_DIR/CAD.js" web/ 2>/dev/null || true
  cp "$BUILD_DIR/CAD.wasm" web/ 2>/dev/null || true
  echo "Built web/hello.html. Serve the 'web' directory (e.g. python -m http.server -d web) and open http://localhost:8000/hello.html"
elif [ -f "$BUILD_DIR/CAD" ]; then
  echo "Warning: CAD.html not found, but CAD executable exists. This might be a build configuration issue."
  ls -la "$BUILD_DIR/"
else
  echo "Error: Neither CAD.html nor CAD executable found in build directory"
  ls -la "$BUILD_DIR/"
  exit 1
fi

# Ensure there's an index.html at the site root so GitHub Pages can serve the app at /
if [ -f web/hello.html ]; then
  cat > web/index.html <<'HTML'
<!doctype html>
<html>
  <head>
    <meta charset="utf-8" />
    <meta http-equiv="refresh" content="0; url=hello.html">
    <title>CAD Hello</title>
  </head>
  <body> 
    <p>Redirecting to <a href="hello.html">hello.html</a>...</p>
  </body>
</html>
HTML
  echo "Built web/index.html (redirects to hello.html)"
fi
