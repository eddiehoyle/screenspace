cmake_minimum_required(VERSION 3.12)
project(screenspace)

set(CMAKE_CXX_STANDARD 11)
set(MAYA_VERSION 2018)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
#set(OpenGL_GL_PREFERENCE GLVND)

find_package(Maya REQUIRED)
#find_package(OpenGL REQUIRED)

add_subdirectory(src)