#include <chrono>
#include <omp.h>

#include "board.h"

/*
 * NOTE: before run this code on the MIC we have to execute the command line: "export LD_LIBRARY_PATH=."
 *        and we have to ensure the presence of "libiomp5.so" on the MIC.
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

    int width = in.m_width;

    in.set_random();

    #ifdef PRINT
    in.print();
    #endif

    // time start
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // #pragma omp parallel num_threads(th_num)
    // {
    // game iteration
    for (int k = 0; k < it_num; ++k) {

        matrix_in = p_in->matrix;
        matrix_out = p_out->matrix;

        // compute the next matrix state
        //
        // note: to allow a perfect linear scan (that implies vectorization) the number of iterations
        //          are slightly more than necessary. the border are then overwritten.
        //
        #pragma omp parallel for num_threads(th_num) schedule(static)
        for (int i = 1; i <= rows; ++i) {

            // current, upper and lower indices
            // current, upper and lower indices
            auto up_p = (i-1)*width + 1;
            auto curr_p = up_p + width;
            auto low_p = curr_p + width;

            #pragma ivdep
            for (int j = 0; j < cols; ++j) {
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
            int left = i*width;
            int right = left+width-1;
            matrix_out[left] = matrix_out[right-1];
            matrix_out[right] = matrix_out[left+1];

            // first row must be copied as bottom border
            if (i == 1) {
                const auto fr_index = width;
                const auto bb_index = (rows+1)*width;
                #pragma ivdep
                for (int z = 0; z < width; ++z) {
                    matrix_out[bb_index+z] = matrix_out[fr_index+z];
                }
            }
            // last row must be copied as bottom border
            if (i == rows) {
                const auto lr_index = (rows)*width;
                #pragma ivdep
                for (int ub_index = 0; ub_index < width; ++ub_index) {
                    matrix_out[ub_index] = matrix_out[lr_index+ub_index];
                }
            }

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
