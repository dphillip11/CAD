include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

# Set GoogleTest build options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "*.cpp")

add_executable(UnitTests ${TEST_SOURCES})
target_link_libraries(UnitTests PRIVATE cad_lib GTest::gtest_main GTest::gtest)

# Ensure consistent compiler flags
target_compile_options(UnitTests PRIVATE -Wno-deprecated-declarations)

# Discover tests
include(GoogleTest)
gtest_discover_tests(UnitTests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Add a manual test target as fallback
add_test(NAME RunAllTests COMMAND UnitTests)
