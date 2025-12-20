cmake_minimum_required(VERSION 3.14)
project(CAD VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(hello STATIC src/hello.cpp)
target_include_directories(hello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(cad_app src/main.cpp)
target_link_libraries(cad_app PRIVATE hello)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.14.0.zip
  )
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  add_executable(hello_test test/test_main.cpp)
  target_link_libraries(hello_test PRIVATE hello gtest_main)
  include(GoogleTest)
  gtest_discover_tests(hello_test)
endif()

## Web packaging helper (optional)
add_custom_target(build_web
  COMMAND ${CMAKE_COMMAND} -E echo "Run ./scripts/build-web.sh to build a browser-ready version (requires Emscripten)."
)
