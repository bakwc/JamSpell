cmake_minimum_required(VERSION 2.8)
project(jamspell)

set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC -g")

find_package(GTest)

link_directories(${PROJECT_BINARY_DIR}/jamspell)
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(jamspell)
add_subdirectory(train)
add_subdirectory(contrib)

if(GTest_FOUND)
    add_subdirectory(tests)
endif()
