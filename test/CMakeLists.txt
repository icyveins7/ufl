cmake_minimum_required(VERSION 3.20)

project(upfirlerp_tests)

add_executable(
    check_against_py
    check_against_py.cpp
)
target_link_libraries(check_against_py PUBLIC Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(check_against_py)

