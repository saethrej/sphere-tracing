set(INSTALLED_TARGETS_LIST "")

# make sure that this was not executed so far
if (NOT TARGET sphere)
    # first add source files that are always built
    set(sphere_src_files SphereShapes.cpp 
                         Scene.cpp
                         ImageClasses.cpp
                         Renderer.cpp
                         CustomVector.cpp
    )
    # the benchmark object is only built if option SPHERE_WITH_BENCHMARKS is chosen
    # if microbenchmarks option is chosen, then SPHERE_WITH_BENCHMARKS is deactivated
    if (SPHERE_WITH_BENCHMARKS AND NOT SPHERE_WITH_MICROBENCHMARKS)
        set (sphere_src_files ${sphere_src_files} SphereBenchmark.cpp)
    endif()

    # the microbenchmark object is only built if option SPHERE_WITH_MICROBENCHMARKS is chosen
    if (SPHERE_WITH_MICROBENCHMARKS)
        set (sphere_src_files ${sphere_src_files} SphereMicroBenchmark.cpp)
    endif()

    # add the accumulated source files to the library
    add_library(sphere ${sphere_src_files})

    # add link-time optimization for this target
    set_property(TARGET sphere PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    
    # specify compiler options that are valid in any case
    target_include_directories(sphere PUBLIC $<BUILD_INTERFACE:${sphere_SOURCE_DIR}/src> 
                                      PUBLIC "${sphere_SOURCE_DIR}/libs/json/single_include/nlohmann/")
    target_compile_options(sphere PRIVATE -O3 -march=native -ffast-math)
    target_compile_features(sphere PUBLIC cxx_std_17)
    target_link_libraries(sphere PRIVATE nlohmann_json::nlohmann_json)

    # set the BENCHMARK macro if in benchmark mode
    if (SPHERE_WITH_BENCHMARKS AND NOT SPHERE_WITH_MICROBENCHMARKS)
        if (BENCHMARKS_COUNT_OP)
            target_compile_definitions(sphere PUBLIC "SPHERE_WITH_BENCHMARKS" "BENCHMARKS_COUNT_OP")
        else()
            target_compile_definitions(sphere PUBLIC "SPHERE_WITH_BENCHMARKS")
        endif()
    endif()

    # set the SPHERE_WITH_MICROBENCHMARKS macro if in microbenchmarking mode
    if (SPHERE_WITH_MICROBENCHMARKS)
        target_compile_definitions(sphere PUBLIC "SPHERE_WITH_MICROBENCHMARKS")
    endif()

    # link to the semiprof library if profiling build option is set
    if (SPHERE_WITH_PROFILING)
        target_link_libraries(sphere PUBLIC semiprof)
        target_compile_definitions(sphere PUBLIC SPHERE_WITH_PROFILING)
    endif()

    # print the current compile flags and definitions
    get_target_property(sphere_compile_flags sphere COMPILE_OPTIONS)
    get_target_property(sphere_compile_defs sphere COMPILE_DEFINITIONS)
    message("-- SPHERE compile definitions: ${sphere_compile_defs}")
    message("-- SPHERE compile flags: ${sphere_compile_flags}")

    list(APPEND INSTALLED_TARGETS_LIST "sphere")
endif()



    