# Fetch Catch2 v3 directly from GitHub
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.14.0
)
FetchContent_MakeAvailable(Catch2)

# Add the test executable
add_executable(rng_tests test_random.cpp)

# Link your library and the Catch2 main entry point
target_link_libraries(rng_tests PRIVATE rng::rng Catch2::Catch2WithMain)

# Register the tests with CTest
include(Catch)
catch_discover_tests(rng_tests)

