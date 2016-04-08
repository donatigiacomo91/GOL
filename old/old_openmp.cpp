#include <iostream>
#include <omp.h>
#include <chrono>

#include "old_board.h"
#include "old_game_logic.h"
#include "old_game_conf.h"

/*
 *
 * compile with: g++-5 openmp.cpp -std=c++11 -O3 -fopenmp -o openmp.exe
 * run with:
 *
 * */

// #define PRINT

int main(int argc, char* argv[]) {

    // board size
    auto rows = atoi(argv[1]);
    auto cols = atoi(argv[2]);
    // iteration number
    auto it_num = atoi(argv[3]);
    auto th_num = atoi(argv[4]);
    // starting configuration
    auto conf_num = (argc>5) ? atoi(argv[5]) : 0;

    // data structures
    old_board in(rows, cols);
    old_board out(rows, cols);
    // data pointers
    old_board * p_in = &in;
    old_board * p_out = &out;

    switch (conf_num) {
        case 0:
            game_conf::set_random_conf(in);
            break;
        case 1 :
            game_conf::set_test_conf_1(in);
            break;
        case 2 :
            game_conf::set_test_conf_2(in);
            break;
        case 3:
            game_conf::set_test_conf_3(in);
            break;
        case 4:
            game_conf::set_test_conf_4(in);
            break;
    }
    #ifdef PRINT
    in.print();
    #endif

    // time start
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // game iteration
    for (int z = 0; z < it_num; ++z) {

        #pragma omp parallel for collapse(2) num_threads(th_num)
        for (auto i = 0; i < rows; ++i) {
            for (auto j = 0; j < cols; ++j) {
                game_logic::update(i, j, *p_in, *p_out);
            }
        }
        
        #ifdef PRINT
        (*p_out).print();
        std::cout << std::endl;
        #endif

        //swap pointer
        old_board *tmp = p_in;
        p_in = p_out;
        p_out = tmp;
    }

    // time end
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << std::endl;
    std::cout << "game execution time is: " << duration << " milliseconds" << std::endl;
    std::cout << std::endl;

    return 0;
}
