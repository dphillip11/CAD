Written in CPP using a graphics API.

Uses cmake to define dependencies.

Can be packaged for web using Emscripten.

Uses google test for testing.


Example: a minimal "Hello World" application

Build and run locally (native build):

1. Create a build directory and configure with CMake:

	mkdir -p build && cd build
	cmake ..

2. Build and run:

	cmake --build . --config Release
	./cad_app

You should see the message "Hello, World! from CAD" printed to stdout.

Build and run in a web browser (Emscripten):

1. Install and activate Emscripten (emsdk) and make sure `emcc` is on your PATH.
	See: https://emscripten.org/docs/getting_started/downloads.html

2. From the project root run:

	./scripts/build-web.sh

3. Serve the generated `web` folder and open the produced `hello.html` in a browser:

	python -m http.server -d web 8000
	# then open http://localhost:8000/hello.html

Tests:

Run unit tests with CTest after building the project (tests use Google Test):

  cmake --build . --target test

Continuous Integration:

This repository includes a GitHub Actions workflow at `.github/workflows/ci.yml` that:

- Builds and tests the native project on Ubuntu and macOS.
- Builds the web artifact using Emscripten on Ubuntu and uploads the `web/` folder as a workflow artifact.

The workflow runs on push and pull requests to `main`.

Pre-commit / Linting

This repository's pre-commit configuration focuses on C/C++ formatting (clang-format).

To enable locally:

	pip install pre-commit
	pre-commit install

To check formatting and run the clang-format hook across the repo:

	pre-commit run --all-files

There is also a convenience script to format all C/C++ files:

	./scripts/format.sh

On CI, a `lint` job runs `pre-commit` and checks C/C++ formatting on pull requests to `main`.

GitHub Pages deployment

When code is pushed to `main` the CI will build the web artifact and deploy the `web/` directory to the `gh-pages` branch. After the first deploy, GitHub Pages will serve the site at:

	https://<your-github-username>.github.io/<repository-name>/

For example: `https://dphillip11.github.io/CAD/`

If you'd prefer to only deploy manually or on release, I can change the workflow to restrict deployment to tags or release events instead.

Note: GitHub sometimes requires enabling Pages in repository settings the first time. If the site does not appear after deployment, go to Settings -> Pages and set the source to the `gh-pages` branch (the workflow will create this branch automatically on the first deployment).

Quick run scripts

To make the common workflows easier there are two convenience scripts:

- Build and run the native app:

		./scripts/build-and-run-local.sh

- Build the web artifact and serve it locally (defaults to port 8000):

		./scripts/build-and-serve-web.sh [port]

Example:

	./scripts/build-and-run-local.sh
	./scripts/build-and-serve-web.sh 8000

Notes on installing tools locally:

- macOS (Homebrew):

	brew install pre-commit clang-format shellcheck

- Ubuntu/Debian:

	sudo apt-get update && sudo apt-get install -y python3-pip clang-format shellcheck
	pip3 install pre-commit

Setup scripts:


To make it easier to prepare a development environment, there are helper scripts in `scripts/`:

- `./scripts/setup-env-ubuntu.sh` — installs build tools and pre-commit on Ubuntu/Debian. Run with `sudo`:

		sudo ./scripts/setup-env-ubuntu.sh

	To install and enable pre-commit hooks for the non-root user in a single step, pass `--install-hooks` when invoking the script with sudo:

		sudo ./scripts/setup-env-ubuntu.sh --install-hooks

- `./scripts/setup-env-macos.sh` — installs tools via Homebrew on macOS (requires Homebrew):

		./scripts/setup-env-macos.sh

	To install and enable pre-commit hooks for the current user in a single step, pass `--install-hooks`:

		./scripts/setup-env-macos.sh --install-hooks

		The macOS script also supports `--autoupdate` to run `pre-commit autoupdate` after installing hooks:

			./scripts/setup-env-macos.sh --install-hooks --autoupdate

	Troubleshooting pre-commit

	If `pre-commit` fails to run with an `InvalidManifestError` (a corrupted cache or bad download), the setup scripts will try to clear the pre-commit cache and retry once. If the problem persists, try these steps:

		rm -rf ~/.cache/pre-commit
		~/.local/bin/pre-commit install
		~/.local/bin/pre-commit run --all-files --show-diff-on-failure

	On macOS, some Python installations restrict `pip install --user`. The scripts will attempt to install `pre-commit` using `pipx` (installed via Homebrew) as a fallback. If you'd prefer, install `pre-commit` yourself with `pipx install pre-commit` or use a virtualenv.

Both scripts will install or instruct how to install `pre-commit`, `clang-format`, and `shellcheck`. They also show optional instructions for installing Emscripten (emsdk).

