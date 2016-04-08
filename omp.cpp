#include <chrono>
#include <omp.h>

#include "board.h"

/*
 * compile with: g++-5 omp2.cpp -std=c++11 -O3 -fopenmp -o omp2.exe
 */

// cache efficient version, the board is extended with additional border to allow
// a linear scan of the memory with tree indices that compute the neighbour sum
//
// the two board (implemented as contiguous memory) are read and write in a perfect linear way

int main(int argc, char* argv[]) {

    // board size
    auto rows = atoi(argv[1]);
    auto cols = atoi(argv[2]);
    // iteration number
    auto it_num = atoi(argv[3]);
    // parallelism degree
    auto th_num = atoi(argv[4]);

    // data structures
    board2 in(rows,cols);
    board2 out(rows,cols);
    // data pointers
    board2* p_in = &in;
    board2* p_out = &out;
    // matrix data pointer;
    int* matrix_in;
    int* matrix_out;

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

        #pragma omp parallel for num_threads(th_num)
        for (int i = 1; i <= rows * cols; ++i) {

            auto sum = matrix_in[up_p-1] + matrix_in[up_p] + matrix_in[up_p+1]
                       + matrix_in[curr_p-1] + matrix_in[curr_p+1]
                       + matrix_in[low_p-1] + matrix_in[low_p] + matrix_in[low_p+1];

            // empty cell
            if (matrix_in[curr_p] == 0) {
                // with exactly 3 alive neighbours alive otherwise keep empty
                matrix_out[curr_p] = (sum == 3) ? 1 : 0;
            } else if (matrix_in[curr_p] == 1) {
                // with less then 2 or more than 3 alive neighbours then die
                // otherwise keep alive
                matrix_out[curr_p] = (sum < 2 || sum > 3) ? 0 : 1;
            }

            up_p = (i%cols == 0) ? up_p+3 : up_p+1;
            curr_p = (i%cols == 0) ? curr_p+3 : curr_p+1;
            low_p = (i%cols == 0) ? low_p+3 : low_p+1;

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
