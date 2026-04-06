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

# ==========================================
# 4. Optional Documentation (Doxygen)
# ==========================================
option(RNG_BUILD_DOCS "Build Doxygen HTML documentation" OFF)

if(RNG_BUILD_DOCS)
  find_package(Doxygen)

  if(DOXYGEN_FOUND)
    # Configure how Doxygen behaves
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_PROJECT_NAME "rng")
    set(DOXYGEN_PROJECT_BRIEF "A modern, ergonomic C++23 random utility library.")

    # This tells Doxygen to ignore your _detail namespace
    set(DOXYGEN_EXCLUDE_SYMBOLS "rng::_detail")

    # Automatically generates the documentation target
    doxygen_add_docs(
            rng_docs
            ${CMAKE_CURRENT_SOURCE_DIR}/include # Path to your source files
            ALL # Adds this to the default build target
            COMMENT "Generating HTML documentation using Doxygen"
        )

    message(STATUS "Doxygen support enabled. Documentation will be built.")
  else()
    message(WARNING "Doxygen requested but not found. Please install Doxygen.")
  endif()
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

