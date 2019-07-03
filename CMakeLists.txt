cmake_minimum_required(VERSION 3.0)
project(screenspace)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugin)

set(MAYA_VERSION 2018)
find_package(Maya REQUIRED)

add_subdirectory(src)