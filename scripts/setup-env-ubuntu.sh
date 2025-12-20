#!/usr/bin/env bash
set -euo pipefail

INSTALL_HOOKS="no"
AUToupDATE="no"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --install-hooks) INSTALL_HOOKS="yes"; shift ;;
    --autoupdate) AUToupDATE="yes"; shift ;;
    *) echo "Unknown arg: $1"; exit 1 ;;
  esac
done

if [ "$EUID" -ne 0 ]; then
  echo "This script must be run with sudo on Ubuntu/Debian to install packages. Example: sudo ./scripts/setup-env-ubuntu.sh [--install-hooks]"
  exit 1
fi

echo "Updating apt repositories..."
apt-get update

echo "Installing system packages: build-essential, cmake, clang-format, shellcheck, python3, python3-pip, git"
apt-get install -y build-essential cmake clang-format shellcheck python3 python3-pip git

echo "Installing pre-commit via pip3 (for non-root user)"
python3 -m pip install --upgrade pip
if [ "$INSTALL_HOOKS" = "yes" ]; then
  if [ -n "${SUDO_USER-}" ]; then
    echo "Installing pre-commit for user $SUDO_USER and enabling hooks"
    su -l "$SUDO_USER" -c 'python3 -m pip install --user pre-commit || true; rm -rf "$HOME/.cache/pre-commit" || true; if ! (command -v ~/.local/bin/pre-commit >/dev/null 2>&1 || command -v pre-commit >/dev/null 2>&1); then python3 -m pip install --user pipx || true; python3 -m pipx ensurepath || true; python3 -m pipx install pre-commit || true; fi; (~/.local/bin/pre-commit install || pre-commit install) || true; if [ "'"$AUToupDATE"'" = "yes" ]; then (~/.local/bin/pre-commit autoupdate || pre-commit autoupdate) || true; fi; if ! (~/.local/bin/pre-commit run --all-files --show-diff-on-failure || pre-commit run --all-files --show-diff-on-failure); then rm -rf "$HOME/.cache/pre-commit" || true; (~/.local/bin/pre-commit install || pre-commit install) || true; (~/.local/bin/pre-commit run --all-files --show-diff-on-failure || pre-commit run --all-files --show-diff-on-failure) || true; fi'
  else
    python3 -m pip install --user pre-commit || true
    rm -rf "$HOME/.cache/pre-commit" || true
    # Fallback to pipx if pre-commit isn't available after pip install
    if ! (command -v ~/.local/bin/pre-commit >/dev/null 2>&1 || command -v pre-commit >/dev/null 2>&1); then
      echo "pre-commit not found after pip install; attempting to install pipx and pre-commit via pipx"
      python3 -m pip install --user pipx || true
      python3 -m pipx ensurepath || true
      python3 -m pipx install pre-commit || true
    fi
    (~/.local/bin/pre-commit install || pre-commit install) || true
    if [ "$AUToupDATE" = "yes" ]; then
      (~/.local/bin/pre-commit autoupdate || pre-commit autoupdate) || true
    fi
    if ! (~/.local/bin/pre-commit run --all-files --show-diff-on-failure || pre-commit run --all-files --show-diff-on-failure); then
      echo "pre-commit run failed on first attempt — clearing cache and retrying once"
      rm -rf "$HOME/.cache/pre-commit" || true
      (~/.local/bin/pre-commit install || pre-commit install) || true
      (~/.local/bin/pre-commit run --all-files --show-diff-on-failure || pre-commit run --all-files --show-diff-on-failure) || true
    fi
  fi
else
  echo "Installing pre-commit (user). Run this script with --install-hooks to automatically enable hooks for the non-root user."
  if [ -n "${SUDO_USER-}" ]; then
    su -l "$SUDO_USER" -c 'python3 -m pip install --user pre-commit || true; rm -rf "$HOME/.cache/pre-commit" || true'
  else
    python3 -m pip install --user pre-commit || true
    rm -rf "$HOME/.cache/pre-commit" || true
  fi
fi

echo "Ensure ~/.local/bin is on your PATH (you may need to restart your shell):"
echo "  export PATH=\"$HOME/.local/bin:$PATH\""

echo "Optional: Install Emscripten SDK (emsdk). To install, run this script with the --with-emsdk flag as root, or run the following as a normal user:"
echo "  git clone https://github.com/emscripten-core/emsdk.git"
echo "  cd emsdk && ./emsdk install latest && ./emsdk activate latest"
echo "  source ./emsdk_env.sh"

echo "Done. You can now run './scripts/format.sh' and 'pre-commit install' in your repo root."
