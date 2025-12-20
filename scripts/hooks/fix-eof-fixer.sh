#!/usr/bin/env bash
set -euo pipefail

# Ensure files end with a single newline
for file in "$@"; do
  if [ -f "$file" ]; then
    # Remove trailing blank lines, then ensure exactly one newline at EOF
    awk ' { sub(/[[:space:]]+$/,"",$0); lines[NR] = $0 } END { for (i=1;i<=NR;i++) print lines[i] }' "$file" > "$file".tmp && mv "$file".tmp "$file"
  fi
done
