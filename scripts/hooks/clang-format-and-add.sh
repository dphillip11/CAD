#!/usr/bin/env bash
set -euo pipefail

# Run clang-format on each provided file and git-add it if modified.
if [ "$#" -eq 0 ]; then
  exit 0
fi

CHANGED=0
for file in "$@"; do
  if [ -f "$file" ]; then
    clang-format -style=file -i "$file" || true
    if ! git diff --quiet -- "$file"; then
      git add -- "$file" || true
      CHANGED=1
    fi
  fi
done

# Return non-zero if files were changed to indicate the commit should continue (pre-commit will allow it since we re-staged files)
exit 0
