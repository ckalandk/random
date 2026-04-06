cmake_minimum_required(VERSION 4.3.0)

project(ntd VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

add_library(rng INTERFACE)
add_library(rng::rng ALIAS rng)

target_compile_features(rng INTERFACE cxx_std_23)

target_sources(rng INTERFACE
  FILE_SET HEADERS
  BASE_DIRS include
  FILES
    include/rng/random.hpp
    include/rng/erased_rnd_engine.hpp
)

# ==========================================
# 3. Optional Testing (Catch2)
# ==========================================
option(RNG_BUILD_TESTS "Build Catch2 tests for the rng library" OFF)
option(RNG_BUILD_EXAMPLES "Build rng examples" OFF)
if(RNG_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(RNG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

install(TARGETS rng
    EXPORT rngTargets
    FILE_SET HEADERS
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT rngTargets
    FILE rngConfig.cmake
    NAMESPACE rng::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rng
)

