#!/usr/bin/env bash
set -euo pipefail

# Format all tracked C/C++ source files with .clang-format

files=$(git ls-files '*.c' '*.cpp' '*.cc' '*.h' '*.hpp' || true)
if [ -z "$files" ]; then
  echo "No C/C++ files to format"
  exit 0
fi

echo "$files" | xargs clang-format -style=file -i

echo "Formatted files:"
for f in $files; do
  echo " - $f"
done
