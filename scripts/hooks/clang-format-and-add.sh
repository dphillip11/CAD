#!/usr/bin/env bash
set -euo pipefail

# Find all C/C++ files
FILES=$(git ls-files '*.c' '*.cpp' '*.cc' '*.h' '*.hpp')

CHANGED=0
for file in $FILES; do
    if [ -f "$file" ]; then
        clang-format -style=file -i "$file" || true
        if ! git diff --quiet -- "$file"; then
            git add -- "$file" || true
            CHANGED=1
        fi
    fi
done

exit 0
