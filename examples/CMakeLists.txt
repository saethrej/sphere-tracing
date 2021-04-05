# create executable for the sphere_miniapp
set(sphere_executables "sphere_miniapp")

foreach(exec ${sphere_executables})
    add_executable(${exec} "${exec}.cpp")
    target_link_libraries(${exec} PRIVATE sphere cxxopts)
    target_include_directories(${exec} PUBLIC "${sphere_SOURCE_DIR}/libs/cxxopts/include/")
    
endforeach()

# here we could create a second executable for a different miniapp
