cmake_minimum_required(VERSION 3.10)

project(boc)

set (CMAKE_CXX_STANDARD 17)

include_directories("component/core/include")
add_subdirectory("component/core")
add_subdirectory("component/bok")
