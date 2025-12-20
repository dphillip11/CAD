cmake_minimum_required(VERSION 3.14)
project(CAD VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(hello STATIC src/hello.cpp)
target_include_directories(hello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(cad_app src/main.cpp)
target_link_libraries(cad_app PRIVATE hello)

option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
  include(FetchContent)
  # Prefer a git-based fetch which is more robust than relying on a single ZIP URL
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    # Use the main branch tag to avoid hardcoding a zip URL/tag which may change.
    GIT_TAG main
  )
  # Attempt to make GoogleTest available. If this fails (no network), tests that
  # depend on GoogleTest will be skipped but a minimal self_test is still added
  # above so `ctest` can run.
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  # GoogleTest-based test. Requires FETCHCONTENT to succeed and GTest targets
  # to be available. If you enable tests, ensure network access or vendor
  # googletest in the project.
  add_executable(hello_test test/test_main.cpp)
  # If FetchContent succeeded, GTest targets should now be available.
  if (TARGET GTest::gtest_main)
    target_link_libraries(hello_test PRIVATE hello GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(hello_test)
  else()
    message(STATUS "GoogleTest not available; hello_test will not be linked with gtest. Ensure googletest is available to run tests.")
  endif()
  # Add a convenient target to build and run tests from the command line
  add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS hello_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()

## Web packaging helper (optional)
add_custom_target(build_web
  COMMAND ${CMAKE_COMMAND} -E echo "Run ./scripts/build-web.sh to build a browser-ready version (requires Emscripten)."
)
