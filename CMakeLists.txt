cmake_minimum_required(VERSION 3.3)
project(GOL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(SEQ sequential.cpp board.h game_conf.h game_logic.h)
add_executable(THR threads.cpp board.h game_conf.h game_logic.h)
add_executable(P_THR pthread.cpp board.h game_conf.h game_logic.h)
add_executable(OMP openmp.cpp board.h game_conf.h game_logic.h)

add_executable(SEQ2 sequential2.cpp board2.h)