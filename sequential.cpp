#include <iostream>
#include <chrono>

#include "board.h"
#include "game_logic.h"
#include "game_conf.h"

/*
 *
 * compile with: g++ -std=c++11 -O3 sequential.cpp -o seq.exe
 * run with: ./seq.exe @row_number @colum_number @iteration_number [@configuration_number (from 1 to 4)]
 *
 * */

//#define PRINT

int main(int argc, char* argv[]) {

    // board size
    auto rows = atoi(argv[1]);
    auto cols = atoi(argv[2]);
    // iteration number
    auto it_num = atoi(argv[3]);
    // starting configuration
    auto conf_num = (argc>4) ? atoi(argv[4]) : 0;

    // data structures
    board in(rows,cols);
    board out(rows,cols);
    // data pointers
    board* p_in = &in;
    board* p_out = &out;

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
    for (int k = 0; k < it_num; ++k) {

        for (auto i = 0; i < rows; ++i) {
            for (auto j = 0; j < cols ; ++j) {
                game_logic::update(i,j,*p_in,*p_out);
            }
        }
        #ifdef PRINT
        (*p_out).print();
        std::cout << std::endl;
        #endif

        // swap pointer
        board* tmp = p_in;
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
