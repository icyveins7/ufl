cmake_minimum_required(VERSION 3.20)

project(upfirlerp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(
    example_main
    example/main.cpp
)

