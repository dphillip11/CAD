#!/usr/bin/env bash
set -euo pipefail

echo "Running clang-format and clang-tidy..."

# Get staged C/C++ files
files=$(git diff --cached --name-only --diff-filter=ACM |
  grep -E '\.(c|cpp|cc|h|hpp)$' || true)

if [[ -z "$files" ]]; then
  echo "No C/C++ files to process."
  exit 0
fi

# -----------------------------
# clang-format
# -----------------------------
echo "Formatting files..."

echo "$files" | tr '\n' '\0' |
  xargs -0 clang-format -style=file -i

# Re-add formatted files
echo "$files" | tr '\n' '\0' |
  xargs -0 git add

# -----------------------------
# clang-tidy
# -----------------------------
echo "Running clang-tidy..."

# clang-tidy requires compile flags
# Prefer compile_commands.json if available
TIDY_FLAGS=""

if [[ -f compile_commands.json ]]; then
  TIDY_FLAGS="-p ."
else
  # Fallback — adjust standard as needed
  TIDY_FLAGS="-- -std=c++20"
fi

lint_failed=0

for file in $files; do
  # Only run clang-tidy on source files (not headers)
  if [[ "$file" =~ \.(c|cpp|cc)$ ]]; then
    echo "Linting $file"
    if ! clang-tidy "$file" $TIDY_FLAGS; then
      lint_failed=1
    fi
  fi
done

if [[ "$lint_failed" -ne 0 ]]; then
  echo ""
  echo "❌ clang-tidy failed. Commit aborted."
  echo "Fix the issues above or suppress with NOLINT where appropriate."
  exit 1
fi

echo "✅ clang-format and clang-tidy passed."
