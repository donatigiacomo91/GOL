cmake_minimum_required(VERSION 3.3)
project(GOL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(SEQ sequential.cpp board.h)
add_executable(THR threads.cpp board.h)
add_executable(TST test.cpp)