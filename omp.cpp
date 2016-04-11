#include <chrono>
#include <omp.h>

#include "board.h"

/*
 * local compile with: g++-5 omp.cpp -std=c++11 -O3 -fopenmp -o omp.exe
 */

// cache efficient version, the board is extended with additional border to allow
// a linear scan of the memory with tree indices that compute the neighbour sum
//
// the two board (implemented as contiguous memory) are read and write in a perfect linear way
//
// this version is also vectorized

// #define PRINT

int main(int argc, char* argv[]) {

    // board size
    auto rows = atoi(argv[1]);
    auto cols = atoi(argv[2]);
    // iteration number
    auto it_num = atoi(argv[3]);
    // parallelism degree
    auto th_num = atoi(argv[4]);

    // data structures
    board in(rows, cols);
    board out(rows, cols);
    // data pointers
    board * p_in = &in;
    board * p_out = &out;
    // memory data pointer
    int* matrix_in;
    int* matrix_out;

    in.set_random();

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

        // vectorization here report a potential speedup of 3.5
        #pragma omp parallel for num_threads(th_num) schedule(static)
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

        // set left and right border
        int left, right;
        // no vectorization here (noncontiguous memory access make it inefficient)
        #pragma omp parallel for num_threads(th_num) schedule(static)
        for (int i = 1; i < (rows+2) ; i++) {
            left = i*in.m_width;
            right = left+in.m_width-1;
            matrix_out[left] = matrix_out[right-1];
            matrix_out[right] = matrix_out[left+1];
        }

        // set top and bottom border
        int start = in.m_width; // second row starting index
        int end = (in.m_height-1)*in.m_width; // last row starting index
        // vectorization here report a potential speedup of 1.2
        #pragma omp parallel for num_threads(th_num) schedule(static)
        #pragma ivdep
        for (int j = 0; j < start; ++j) {
            // copy last real row in upper border (first row)
            matrix_out[j] = matrix_out[end-in.m_width+j];
            // copy first real row in bottom border (last row)
            matrix_out[end+j] = matrix_out[start+j];
        }

        #ifdef PRINT
        (*p_out).print();
        std::cout << std::endl;
        #endif

        // swap pointer
        board * tmp = p_in;
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
