cmake_minimum_required(VERSION 3.3)
project(GOL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(OLD_SEQ old/old_sequential.cpp old/old_board.h old/old_game_conf.h old/old_game_logic.h)
add_executable(OLD_THR old/old_threads.cpp old/old_board.h old/old_game_conf.h old/old_game_logic.h)
add_executable(OLD_P_THR old/old_pthread.cpp old/old_board.h old/old_game_conf.h old/old_game_logic.h)
add_executable(OLD_OMP old/old_openmp.cpp old/old_board.h old/old_game_conf.h old/old_game_logic.h)

add_executable(SEQ sequential.cpp board.h)
add_executable(OMP omp.cpp board.h)
add_executable(PTHR pthread.cpp board.h)
add_executable(FF fastflow.cpp board.h)
add_executable(FARM_FF farm_fastflow.cpp board.h)