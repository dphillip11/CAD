#!/usr/bin/env bash
set -euo pipefail

if ! command -v brew >/dev/null 2>&1; then
  echo "Homebrew not found. Install Homebrew first: https://brew.sh/"
  exit 1
fi

echo "Updating Homebrew..."
brew update

echo "Installing packages: cmake, clang-format, shellcheck, python"
brew install cmake clang-format shellcheck python

INSTALL_HOOKS="no"
AUToupDATE="no"
while [[ $# -gt 0 ]]; do
  case "$1" in
    --install-hooks) INSTALL_HOOKS="yes"; shift ;;
    --autoupdate) AUToupDATE="yes"; shift ;;
    *) echo "Unknown arg: $1"; exit 1 ;;
  esac
done

echo "Installing pre-commit (pip user-install)"
python3 -m pip install --user --upgrade pip || true
python3 -m pip install --user pre-commit || true

# Clear any potentially-broken pre-commit cache which can cause InvalidManifestError
if [ -d "$HOME/.cache/pre-commit" ]; then
  echo "Removing pre-commit cache at $HOME/.cache/pre-commit"
  rm -rf "$HOME/.cache/pre-commit" || true
fi

if [ "$AUToupDATE" = "yes" ]; then
  echo "Attempting to autoupdate configured hooks"
  (~/.local/bin/pre-commit autoupdate || pre-commit autoupdate) || true
fi

if [ "$INSTALL_HOOKS" = "yes" ]; then
  echo "Enabling pre-commit hooks for current user"
  # If the user-installed pre-commit isn't available, try installing via pipx as a fallback
  if ! (command -v ~/.local/bin/pre-commit >/dev/null 2>&1 || command -v pre-commit >/dev/null 2>&1); then
    echo "pre-commit not found after pip install; attempting to install via pipx"
    if ! command -v pipx >/dev/null 2>&1; then
      echo "Installing pipx via Homebrew"
      brew install pipx || true
      pipx ensurepath || true
    fi
    if command -v pipx >/dev/null 2>&1; then
      pipx install pre-commit || true
    else
      echo "pipx not available; pre-commit may not be installed. You can install pre-commit manually with 'python3 -m pip install --user pre-commit' or 'pipx install pre-commit'"
    fi
  fi

  (~/.local/bin/pre-commit install || pre-commit install) || true

  echo "Running pre-commit against the repository (may produce diffs)"
  if ! (~/.local/bin/pre-commit run --all-files --show-diff-on-failure || pre-commit run --all-files --show-diff-on-failure); then
    echo "pre-commit run failed on first attempt — clearing cache and retrying once"
    rm -rf "$HOME/.cache/pre-commit" || true
    (~/.local/bin/pre-commit install || pre-commit install) || true
    if ! (~/.local/bin/pre-commit run --all-files --show-diff-on-failure || pre-commit run --all-files --show-diff-on-failure); then
      echo "pre-commit still failed after retry. Check the log at $HOME/.cache/pre-commit/pre-commit.log for details. You can run '~/.local/bin/pre-commit run --all-files' to see problems."
    fi
  fi
else
  echo "pre-commit installed for the user. To enable hooks run: '~/.local/bin/pre-commit install' or run this script with --install-hooks"
fi

echo "Ensure ~/.local/bin is on your PATH (you may need to restart your shell):"
echo "  export PATH=\"$HOME/.local/bin:$PATH\""

echo "Optional: Install Emscripten SDK (emsdk). See https://emscripten.org/docs/getting_started/downloads.html"

echo "Done. You can now run './scripts/format.sh' and 'pre-commit install' in your repo root."
