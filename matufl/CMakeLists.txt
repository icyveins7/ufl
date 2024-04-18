project(upfirlerp_mex)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Matlab)

matlab_add_mex(NAME matufl SRC matufl.cpp R2018a)

