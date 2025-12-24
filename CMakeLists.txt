cmake_minimum_required(VERSION 3.29.2)
project(CAD VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# ---------------------------
# Core library
# ---------------------------
file(GLOB_RECURSE CAD_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_library(cad_lib ${CAD_SOURCES})
target_include_directories(cad_lib PUBLIC src)

# ---------------------------
# Application executable
# ---------------------------
add_executable(CAD main.cpp)
target_link_libraries(CAD PRIVATE cad_lib)

# ---------------------------
# Tests
# ---------------------------
add_subdirectory(test)
