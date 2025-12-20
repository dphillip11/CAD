#!/usr/bin/env bash
set -euo pipefail

# Build the web artifact and serve it locally.
# Usage: ./scripts/build-and-serve-web.sh [port]

PORT=${1:-8000}

echo "Building web artifact (will use emcc)"
./scripts/build-web.sh

echo "Serving 'web' on http://localhost:${PORT}/"

# If possible, open the browser shortly after starting the server.
open_cmd=""
if command -v xdg-open >/dev/null 2>&1; then
  open_cmd="xdg-open"
elif command -v open >/dev/null 2>&1; then
  open_cmd="open"
fi

if [ -n "$open_cmd" ]; then
  (sleep 1; $open_cmd "http://localhost:${PORT}/hello.html" ) &
fi

if command -v python3 >/dev/null 2>&1; then
  python3 -m http.server -d web "$PORT"
else
  python -m http.server -d web "$PORT"
fi
