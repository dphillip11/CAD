include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "*.cpp")

add_executable(UnitTests ${TEST_SOURCES})
target_link_libraries(UnitTests PRIVATE cad_lib gtest_main)

include(GoogleTest)
gtest_discover_tests(UnitTests)
