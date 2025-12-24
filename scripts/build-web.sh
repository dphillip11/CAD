#!/usr/bin/env bash
set -euo pipefail

# Build a browser-ready HTML + WASM file using Emscripten.
# Usage: source /path/to/emsdk/emsdk_env.sh; ./scripts/build-web.sh

if ! command -v emcc >/dev/null 2>&1; then
  echo "emcc not found. Install and activate Emscripten (see https://emscripten.org/docs/getting_started/downloads.html)"
  exit 1
fi

mkdir -p web
echo "Compiling src/main.cpp, src/hello.cpp and src/triangle.cpp -> web/hello.html"
emcc src/main.cpp src/hello.cpp src/triangle.cpp -o web/hello.html -s WASM=1 -O2 -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']" -s ENVIRONMENT='web'
echo "Built web/hello.html. Serve the 'web' directory (e.g. python -m http.server -d web) and open http://localhost:8000/hello.html"

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
