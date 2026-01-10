# CAD Project Research Document

## Project Overview
- **Name**: CAD (Computer-Aided Design)
- **Language**: C++23
- **Build System**: CMake
- **Graphics**: OpenGL/WebGL (dual native and web targets)
- **Testing**: GoogleTest
- **Web Deployment**: Emscripten
- **Repository**: https://github.com/dphillip11/CAD (main branch)

## Architecture Layers (Bottom to Top)
```
┌────────────────────────────┐
│        Rendering           │  GPU, draw calls, buffers
├────────────────────────────┤
│        Primitives          │  Topology & validation
├────────────────────────────┤
│        Geometry            │  Math on positions, no knowledge of topological structs
├────────────────────────────┤
│        Storage             │  Dense/Sparse containers
├────────────────────────────┤
│        Core Types          │  Vec3, IDs, POD structs
└────────────────────────────┘
```

## Key Design Principles
- **Separation of Concerns**: Topology (what exists) ↔ Geometry (where it is) ↔ Rendering (how it is drawn)
- **No IDs/Primitives in Geometry**
- **No Vec3 math in Topology**
- **No validation in data structs**
- **No rendering outside Renderer**
- **Templates live where they are visible**

## Directory Structure

### Source Code (`src/`)
- `App/` - Application entry point and main loop
- `Core/` - Fundamental types (Vec3, AABB, Id, Primitives)
- `Geometry/` - Mathematical operations (Colinearity, Coplanarity, Epsilon)
- `Model/` - CAD model, edit commands, undo/redo, change events
- `ModelView/` - View representations of model entities
- `Rendering/` - GPU rendering, passes, buffers, pipelines
- `Topology/` - Vertex, Edge, Face, Volume primitives and validation
- `Utilities/` - Helper utilities

### Build & Config
- `build/` - CMake build directory (generated)
- `CMakeLists.txt` - Main CMake configuration
- `scripts/` - Build and utility scripts

### Testing
- `test/` - Unit tests (GoogleTest)
- `_deps/` - Dependencies (GoogleTest source/build)

### Web
- `web/` - Emscripten build output (CAD.js, index.html)

## Build Commands

### Native Build (Debug/Release)
```bash
# Configure
cmake -S . -B build -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B build -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Debug -- -j $(nproc)
cmake --build build --config Release -- -j $(nproc)

# Run
./build/CAD
```

### Web Build (Emscripten)
```bash
# Requires emsdk activated
./scripts/build-web.sh
python -m http.server -d web 8000
# Open http://localhost:8000/index.html
```

### Convenience Scripts
- `./scripts/build-and-run-local.sh [debug|release]` - Build and run native
- `./scripts/build-and-serve-web.sh` - Build web and serve
- `./scripts/format.sh` - Format C++ code with clang-format

## Testing
```bash
# Configure with tests
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug

# Run tests
cd build
ctest --output-on-failure -C Debug
# OR
cmake --build build --target check
```

## VS Code Tasks Available
- `CMake: Configure Debug` - Configure CMake for debug build
- `CMake: Build Debug` - Build debug version
- `CMake: Configure Release` - Configure CMake for release build
- `CMake: Build Release` - Build release version
- `C/C++: clang build active file` - Build single C++ file

## Key Files & Entry Points
- `main.cpp` - Application entry point
- `src/App/Application.h/cpp` - Main application class
- `src/Model/CADModel.h/cpp` - Core CAD model
- `src/Rendering/Renderer.h/cpp` - Rendering system
- `src/Rendering/Resources/RenderResources.h/cpp` - Loading of render resources, configuration of render passes
- `src/Rendering/Resources/UniformBuffer.h` - includes the inject uniforms accessible to shader code
- `ARCHITECTURE.md` - Detailed architecture documentation
- `ENVIRONMENT.MD` - Setup and build instructions

## Platform-Specific Code
- Native: `src/Rendering/Devices/RenderDevice_Native.cpp`
- Web: `src/Rendering/Devices/RenderDevice_Emscripten.cpp`

## Dependencies
- **Native**: OpenGL, GLFW, GLEW
- **Web**: Emscripten (provides WebGL, GLFW)
- **Testing**: GoogleTest (fetched via CMake)

## Development Workflow
1. **Setup**: Run `./scripts/setup-env-macos.sh` (macOS) or `./scripts/setup-env-ubuntu.sh` (Linux)
2. **Format**: `./scripts/format.sh` or pre-commit hooks
3. **Build**: Use VS Code tasks or `./scripts/build-and-run-local.sh`
4. **Test**: `cmake --build build --target check`
5. **Web**: `./scripts/build-and-serve-web.sh`

## Useful Searches & Patterns
- **Find all .cpp files**: `src/**/*.cpp`
- **Find all headers**: `src/**/*.h`
- **Test files**: `test/**/*.cpp`
- **Shader files**: `src/Rendering/GLSL/**/*`
- **Build scripts**: `scripts/**/*.sh`

## Common CMake Variables
- `CMAKE_BUILD_TYPE`: Debug/Release
- `BUILD_TESTS`: ON/OFF
- `USE_OPENGL`: ON/OFF
- `EMSCRIPTEN`: Automatically set by emcmake

## CI/CD
- GitHub Actions workflow builds native (Ubuntu/macOS) and web (Ubuntu)
- Pre-commit hooks for clang-format
- Deploys web build to GitHub Pages on main branch push

## Current Development Notes
- WebGL compatibility issues with geometry shaders
- shader code and features must be supported by webGL, this also constrains use of textures
- Face translation affects connected geometry
- Potential for procedural meshes, sculpting, rotational extrusion
- Considering Catmull-Clark subdivision surfaces