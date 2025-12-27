include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

# Set GoogleTest build options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "*.cpp")

add_executable(UnitTests ${TEST_SOURCES})
target_link_libraries(UnitTests PRIVATE cad_lib gtest_main gtest)

# Ensure consistent compiler flags
target_compile_options(UnitTests PRIVATE -Wno-deprecated-declarations)

include(GoogleTest)
gtest_discover_tests(UnitTests)
