cmake_minimum_required(VERSION 3.21)

project(hyt_common)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(HYT_BUILD_SHARED "Build library as shared" ON)
option(HYT_BUILD_DEMO "Build demo program" ON)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(src)
if(HYT_BUILD_DEMO)
    add_subdirectory(demo)
endif(HYT_BUILD_DEMO)
