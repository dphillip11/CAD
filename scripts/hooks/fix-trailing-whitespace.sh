#!/usr/bin/env bash
set -euo pipefail

# Remove trailing whitespace from files passed as arguments
for file in "$@"; do
  if [ -f "$file" ]; then
    sed -i.bak -E 's/[[:space:]]+$//' "$file" && rm -f "$file".bak
  fi
done
