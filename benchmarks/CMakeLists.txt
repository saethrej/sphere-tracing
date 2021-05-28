# create executable for the sphere_benchmarks
if (SPHERE_WITH_BENCHMARKS AND NOT SPHERE_WITH_MICROBENCHMARKS)
    set(sphere_benchmark_executables "sphere_benchmark")
elseif (SPHERE_WITH_MICROBENCHMARKS)
    set(sphere_benchmark_executables "sphere_microbenchmark")
endif()

foreach(exec ${sphere_benchmark_executables})
    # add executable to the queue
    add_executable(${exec} "${exec}.cpp")

    # adapt compile definitions and link libraries depending on whether the executable
    # is for benchmarking or microbenchmarking
    if (SPHERE_WITH_BENCHMARKS AND NOT SPHERE_WITH_MICROBENCHMARKS)
        if (BENCHMARKS_COUNT_OP)
            target_compile_definitions(${exec} PUBLIC SPHERE_WITH_BENCHMARKS BENCHMARKS_COUNT_OP)
        else()
            target_compile_definitions(${exec} PUBLIC SPHERE_WITH_BENCHMARKS)
        endif()
        target_link_libraries(${exec} PRIVATE sphere cxxopts)
        target_include_directories(${exec} PUBLIC "${sphere_SOURCE_DIR}/libs/cxxopts/include/")

    elseif (SPHERE_WITH_MICROBENCHMARKS)
        target_compile_definitions(${exec} PUBLIC SPHERE_WITH_MICROBENCHMARKS)
        target_link_libraries(${exec} PRIVATE sphere)
    endif()
    
endforeach()
