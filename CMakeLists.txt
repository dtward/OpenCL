cmake_minimum_required (VERSION 2.6) # from tutorial

project ( opencl_array )


find_package (OpenCL)
add_library( opencl_array opencl_array.cpp )
include_directories( ${OpenCL_INCLUDE_DIRS} )
target_link_libraries ( opencl_array ${OpenCL_LIBRARIES} )


add_executable( main main.cpp )
target_link_libraries ( main opencl_array)
