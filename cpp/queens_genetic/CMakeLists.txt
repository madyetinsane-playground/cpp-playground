cmake_minimum_required(VERSION 2.6)
project(queens_genetic)

add_executable(queens_genetic main.cpp)

install(TARGETS queens_genetic RUNTIME DESTINATION bin)
set (CMAKE_CXX_COMPILER             "/usr/lib/clang-analyzer/scan-build/c++-analyzer")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++1y -Wall -O2")