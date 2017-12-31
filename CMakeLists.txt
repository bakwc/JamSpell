cmake_minimum_required(VERSION 2.8)
project(openspell)

set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC -g")

link_directories(${PROJECT_BINARY_DIR}/openspell)
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(openspell)
add_subdirectory(train)
add_subdirectory(contrib)
