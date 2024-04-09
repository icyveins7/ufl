cmake_minimum_required(VERSION 3.20)

project(upfirlerp_benchmarks)

set(CMAKE_BUILD_TYPE Release)
message("Benchmarks BUILD_TYPE defaulting to " ${CMAKE_BUILD_TYPE})

add_executable(
    basic
    basic.cpp
)
target_link_libraries(basic PUBLIC Catch2::Catch2WithMain)

include(CTest)
include(Catch)

