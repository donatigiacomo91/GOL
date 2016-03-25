#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "board.h"

/*
 *
 *
 * compile with: g++-5 threads.cpp -std=c++11 -O3 -o threads.exe
 * run with: ./threads.exe @row_number @colum_number @iteration_number @parallelism degree
 *
 *
 * */

// synchronization lock (some kind of barrier)
std::mutex m;
std::condition_variable cv;
// synchronization variable
int count = 0;
int thread_number;

void update(int i, int j, board& in, board& out) {

    // upper and lower row indices
    auto up_p = (i-1) >= 0 ? (i-1) : (in.m_height-1);
    auto low_p = (i+1) % in.m_height;
    // left and right column indices
    auto left_p = (j-1) >= 0 ? (j-1) : (in.m_width-1);
    auto right_p = (j+1) % in.m_width;

    auto sum = in[up_p][left_p] + in[up_p][j] + in[up_p][right_p]
               + in[i][left_p] + in[i][right_p]
               + in[low_p][left_p] + in[low_p][j] + in[low_p][right_p];

    // empty cell
    if (in[i][j] == 0) {
        // with exactly 3 alive neighbours alive otherwise die
        out[i][j] = (sum == 3) ? 1 : 0;
        return;
    }
    // alive cell
    if (in[i][j] == 1) {
        // with less then 2 or more than 3 alive neighbours then die
        // otherwise keep alive
        out[i][j] = (sum < 2 || sum > 3) ? 0 : 1;
        return;
    }
}

void body(int start, int stop, int iter, board* p_in, board* p_out) {
    // columns number
    const auto col = p_in->m_width;
    // synchronization lock (some kind of barrier)
    std::unique_lock<std::mutex> barrier(m, std::defer_lock);

    // game iteration
    while(iter > 0) {

        for (auto i = start; i <= stop; ++i) {
            for (auto j = 0; j < col; ++j) {
                update(i,j,*p_in,*p_out);
            }
        }

        // swap pointer
        board* tmp = p_in;
        p_in = p_out;
        p_out = tmp;
        // decrease iteration count
        iter--;

        barrier.lock();
        count++;
        if (count == thread_number) {
            // if all thread have complete unlock and notify all
            count = 0;
            barrier.unlock();
            cv.notify_all();
        } else {
            // wait until all thread complete the current game iteration
            cv.wait(barrier);
            barrier.unlock();
        }

    }

    return;
}

int main(int argc, char* argv[]) {

    // board size
    auto rows = atoi(argv[1]);
    auto cols = atoi(argv[2]);
    // iteration number
    auto it_num = atoi(argv[3]);
    // parallelism degree
    auto th_num = atoi(argv[4]);
    // set global var
    thread_number = th_num;

    // data structures
    board in(rows,cols);
    board out(rows,cols);

    // TODO: think about static splitting [particular case es: th_rows<1 ...]

    auto th_rows = rows / th_num;
    auto remains = rows % th_num;

    // thread pool
    std::vector<std::thread> tid;
    int start, stop = 0;
    for(auto i=0; i<th_num; i++) {
        start = stop;
        stop = (remains > 0) ? start + th_rows : start + th_rows -1;
        tid.push_back(std::thread(body, start, stop, it_num, &in, &out));
        remains--;
        stop++;
    }

    // await termination
    for(int i=0; i<th_num; i++)
        tid[i].join();

    return 0;
}
