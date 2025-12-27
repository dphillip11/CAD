cmake_minimum_required(VERSION 3.29.2)
project(CAD VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# ---------------------------
# Dependencies
# ---------------------------
if(EMSCRIPTEN)
    # Emscripten has built-in OpenGL ES support
    set(OPENGL_LIBRARIES "")
    set(GLFW_LIBRARIES "")
else()
    # Native build - find OpenGL, GLFW, and GLEW
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    set(OPENGL_LIBRARIES OpenGL::GL glfw GLEW::GLEW)
    set(GLFW_LIBRARIES glfw)
endif()

# ---------------------------
# Core library
# ---------------------------
file(GLOB_RECURSE CAD_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_library(cad_lib ${CAD_SOURCES})
target_include_directories(cad_lib PUBLIC src)
target_link_libraries(cad_lib PUBLIC ${OPENGL_LIBRARIES})

# Enable OpenGL by default (can be disabled with -DUSE_OPENGL=OFF)
option(USE_OPENGL "Use OpenGL rendering" ON)
if(USE_OPENGL)
    target_compile_definitions(cad_lib PUBLIC USE_OPENGL)
endif()

# ---------------------------
# Application executable
# ---------------------------
add_executable(CAD main.cpp)
target_link_libraries(CAD PRIVATE cad_lib)
if(NOT EMSCRIPTEN)
    target_link_libraries(CAD PRIVATE ${GLFW_LIBRARIES})
endif()

# Emscripten-specific settings
if(EMSCRIPTEN)
    set_target_properties(CAD PROPERTIES 
        SUFFIX ".html"
        LINK_FLAGS "-s WASM=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1"
    )
endif()

# ---------------------------
# Tests
# ---------------------------
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    add_subdirectory(test)
endif()
