cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# check if this project is a sub-project or the master project
set(MASTER_PROJECT OFF)
if (NOT DEFINED PROJECT_NAME)
    set(MASTER_PROJECT ON)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/build_type.cmake)

# build options
option(SPHERE_WITH_TESTS "Generate the test targets." OFF)
option(SPHERE_WITH_APPS "Generate the example targets." ${MASTER_PROJECT})
option(SPHERE_WITH_BENCHMARKS "Generate the benchmark targets." OFF)
option(SPHERE_WITH_MICROBENCHMARKS "Generate microbenchmark targets" OFF)
option(SPHERE_WITH_PROFILING "Enables profiling." OFF)
option(SPHERE_WITH_OPENMP "Enables the usage of OpenMP in SPHERE" ON)
option(BUILD_SHARED_LIBS "Build shared libraries." OFF)

# set project options
project(sphere VERSION 0.1 LANGUAGES CXX)

# set macro DEBUG in debug build mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# if the library is to be built with OpenMP, check if it is available
# note that in the presence of Microbenchmarks, OpenMP cannot be used
if (SPHERE_WITH_OPENMP AND NOT SPHERE_WITH_MICROBENCHMARKS)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -DSPHERE_WITH_OPENMP")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DSPHERE_WITH_OPENMP")
    endif()
endif()

# set-up bundled dependencies:
# the json library will be included in every build
OPTION(JSON_BuildTests OFF)
OPTION(JSON_Install OFF)
add_subdirectory(libs/json)

# semiprof is only included in profiling mode
if (SPHERE_WITH_PROFILING)
    add_subdirectory(libs/semiprof)
endif()
# cxxopts is included whenever a test/app/benchmark suite is built
if (SPHERE_WITH_TESTS OR SPHERE_WITH_APPS OR SPHERE_WITH_BENCHMARKS)
    option(CXXOPTS_BUILD_EXAMPLES OFF)
    option(CXXOPTS_BUILD_TESTS OFF)
    option(CXXOPTS_ENABLE_INSTALL OFF)
    add_subdirectory(libs/cxxopts)
endif()

# add the source code 
add_subdirectory(src/sphere)

# if tests are enabled, add the tests directory
if (SPHERE_WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# if apps (i.e. examples) are enabled, add the directory
if (SPHERE_WITH_APPS)
    add_subdirectory(examples)
endif()

# if (micro)benchmarks are enabled, add the directory
if (SPHERE_WITH_BENCHMARKS OR SPHERE_WITH_MICROBENCHMARKS)
    add_subdirectory(benchmarks)
endif()
