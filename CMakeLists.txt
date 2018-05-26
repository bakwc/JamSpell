cmake_minimum_required(VERSION 2.8)
project(jamspell)

option(USE_BOOST_CONVERT "use Boost.Locale instead of std::codecvt for string conversion" OFF)

set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC -g")

find_package(GTest)

link_directories(${PROJECT_BINARY_DIR}/jamspell)
include_directories(${CMAKE_SOURCE_DIR})

# find Boost if necessary
if(USE_BOOST_CONVERT)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    find_package(Boost REQUIRED)
    message(STATUS "Using Boost string convert: Enabled")
    add_definitions(-DUSE_BOOST_CONVERT)
endif()

find_package (Threads)

add_subdirectory(jamspell)
add_subdirectory(main)
add_subdirectory(contrib)
add_subdirectory(web_server)

if(GTest_FOUND)
    add_subdirectory(tests)
endif()
