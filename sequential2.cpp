#include "board2.h"
#include <chrono>

#include <iostream>

/*
 * compile with: g++ -std=c++11 -O3 sequential2.cpp -o seq2.exe
 */

// cache efficient version, the board is extended with additional border to allow
// a linear scan of the memory with tree indices that compute the neighbour sum
//
// the two board (implemented as contiguous memory) are read and write in a perfect linear way

//#define PRINT

int main(int argc, char* argv[]) {

    // board size
    auto rows = atoi(argv[1]);
    auto cols = atoi(argv[2]);
    // iteration number
    auto it_num = atoi(argv[3]);
    // starting configuration
    //auto conf_num = (argc>4) ? atoi(argv[4]) : 0;

    // data structures
    board2 in(rows,cols);
    board2 out(rows,cols);
    // data pointers
    board2* p_in = &in;
    board2* p_out = &out;
    // matrix data pointer;
    int* matrix_in;
    int* matrix_out;

//    switch (conf_num) {
//        case 0:
//            game_conf::set_random_conf(in);
//            break;
//        case 1 :
//            game_conf::set_test_conf_1(in);
//            break;
//        case 2 :
//            game_conf::set_test_conf_2(in);
//            break;
//        case 3:
//            game_conf::set_test_conf_3(in);
//            break;
//        case 4:
//            game_conf::set_test_conf_4(in);
//            break;
//    }
#ifdef PRINT
    in.print();
#endif

    // time start
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // game iteration
    for (int k = 0; k < it_num; ++k) {

        // current, upper and lower indices
        auto up_p = 1;
        auto curr_p = in.m_width+1;
        auto low_p = in.m_width*2+1;

        matrix_in = p_in->matrix;
        matrix_out = p_out->matrix;

//        for (int i = 1; i <= rows * cols; ++i) {
//
//            // compute alive neighbours
//            auto sum = matrix_in[up_p-1] + matrix_in[up_p] + matrix_in[up_p+1]
//                        + matrix_in[curr_p-1] + matrix_in[curr_p+1]
//                        + matrix_in[low_p-1] + matrix_in[low_p] + matrix_in[low_p+1];
//
//            // set the current cell state
//            matrix_out[curr_p] = (sum == 3) || (sum+matrix_in[curr_p] == 3) ? 1 : 0;
//
//            // move the pointers (by 3 to skip the border computation)
//            up_p = (i%cols == 0) ? up_p+3 : up_p+1;
//            curr_p = (i%cols == 0) ? curr_p+3 : curr_p+1;
//            low_p = (i%cols == 0) ? low_p+3 : low_p+1;
//
//        }

	#pragma ivdep
        for (int i = 0; i < (cols+2) * rows; ++i) {

            // compute alive neighbours
            auto sum = matrix_in[up_p-1] + matrix_in[up_p] + matrix_in[up_p+1]
                       + matrix_in[curr_p-1] + matrix_in[curr_p+1]
                       + matrix_in[low_p-1] + matrix_in[low_p] + matrix_in[low_p+1];

            // set the current cell state
            matrix_out[curr_p] = (sum == 3) || (sum+matrix_in[curr_p] == 3) ? 1 : 0;

            // move the pointers
            up_p++;
            curr_p++;
            low_p++;

        }

        // fill left and right border
        int left, right;
        for (int i = 1; i < in.m_height ; i++) {
            left = i*in.m_width;
            right = left+in.m_width-1;
            matrix_out[left] = matrix_out[right-1];
            matrix_out[right] = matrix_out[left+1];
        }

        // fill top and bottom border
        int start = in.m_width; // second row starting index
        int end = (in.m_height-1)*in.m_width; // last row starting index
        for (int j = 0; j < start; ++j) {
            // copy last row in first row
            matrix_out[j] = matrix_out[end-in.m_width+j];
            // copy first row in last row
            matrix_out[end+j] = matrix_out[start+j];
        }

#ifdef PRINT
        (*p_out).print();
        std::cout << std::endl;
#endif

        // swap pointer
        board2* tmp = p_in;
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
