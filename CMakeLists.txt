cmake_minimum_required(VERSION 3.16)
project(DigitalLibraryEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# Include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Collect library sources
file(GLOB_RECURSE DLS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Core static library
add_library(dls_core STATIC ${DLS_SOURCES})

# Main CLI Application
add_executable(dls_app app/main.cpp)
target_link_libraries(dls_app PRIVATE dls_core)

# Benchmark Executable
add_executable(dls_bench bench/benchmark.cpp)
target_link_libraries(dls_bench PRIVATE dls_core)

# Tests
enable_testing()

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE dls_core)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
