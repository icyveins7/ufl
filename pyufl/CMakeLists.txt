cmake_minimum_required(VERSION 3.20)

project(upfirlerp_nanobind)

find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)

# Force Release build for nanobind
# set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
set(CMAKE_BUILD_TYPE ${NANOBIND_BUILD_TYPE})
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
message("Nanobind build type is now " ${CMAKE_BUILD_TYPE})

# Detect the installed nanobind package and import it into CMake
execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE NB_DIR)
list(APPEND CMAKE_PREFIX_PATH "${NB_DIR}")
find_package(nanobind CONFIG REQUIRED)

# Add the module
nanobind_add_module(
    pyufl NOMINSIZE
    pyufl.cpp
)
