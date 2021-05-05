# create executable for the sphere_benchmarks
set(sphere_benchmark_executables "sphere_benchmark")

foreach(exec ${sphere_benchmark_executables})
    # in this executable, the SPHERE_WITH_BENCHMARKS is set anyway
    # but we might also have to set the BENCHMARKS_COUNT_OP macro
    add_executable(${exec} "${exec}.cpp")
    if (BENCHMARKS_COUNT_OP)
        target_compile_definitions(${exec} PUBLIC SPHERE_WITH_BENCHMARKS BENCHMARKS_COUNT_OP)
    else()
        target_compile_definitions(${exec} PUBLIC SPHERE_WITH_BENCHMARKS)
    endif()
    target_link_libraries(${exec} PRIVATE sphere cxxopts)
    target_include_directories(${exec} PUBLIC "${sphere_SOURCE_DIR}/libs/cxxopts/include/")
    
endforeach()
