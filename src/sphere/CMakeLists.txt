set(INSTALLED_TARGETS_LIST "")

# make sure that this was not executed so far
if (NOT TARGET sphere)
    set(sphere_src_files SphereShapes.cpp 
                         Scene.cpp  
    )
    add_library(sphere ${sphere_src_files})
    
    # specify compiler options that are valid in any case
    target_include_directories(sphere PUBLIC $<BUILD_INTERFACE:${sphere_SOURCE_DIR}/src> 
                                      PUBLIC "${sphere_SOURCE_DIR}/libs/json/single_include/nlohmann/")
    target_compile_options(sphere PRIVATE -O3 PRIVATE -march=native)
    target_compile_features(sphere PUBLIC cxx_std_17)
    target_link_libraries(sphere PRIVATE nlohmann_json::nlohmann_json)

    # set the BENCHMARK macro if in benchmark mode
    if (SPHERE_WITH_BENCHMARKS)
        target_compile_definitions(sphere PUBLIC "BENCHMARK")
    endif()

    # link to the semiprof library if profiling build option is set
    if (SPHERE_WITH_PROFILING)
        target_link_libraries(sphere PUBLIC semiprof)
        target_compile_definitions(sphere PUBLIC SPHERE_WITH_PROFILING)
    endif()

    list(APPEND INSTALLED_TARGETS_LIST "sphere")
endif()



    