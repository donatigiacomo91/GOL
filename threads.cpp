#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "board.h"
#include "game_conf.h"

/*
 *
 *
 * compile with: g++ threads.cpp -std=c++11 -O3 -o threads.exe
 * run with: ./threads.exe @row_number @colum_number @iteration_number @parallelism degree [@configuration_number (from 1 to 4)]
 *
 *
 * */

// #define PRINT

// game data
board* in;
board* out;
int game_iteration;

// synchronization lock (some kind of barrier)
std::mutex m;
std::condition_variable cv;

// synchronization variable
int barrier_count = 0;
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

void body(int start, int stop) {

    //std::cout << "Thread n. " << std::this_thread::get_id() << " start executing..."<< std::endl;

    const auto col = in->m_width;
    board* p_in = in;
    board* p_out = out;

    // TODO: this must be in the main (one time initialization) ???
    // synchronization lock (some kind of barrier)
    std::unique_lock<std::mutex> barrier(m, std::defer_lock);

    // game iteration
    for (int k = 0; k < game_iteration; ++k) {

        for (auto i = start; i <= stop; ++i) {
            for (auto j = 0; j < col; ++j) {
                update(i,j,*p_in,*p_out);
            }
        }

        // swap pointer
        board* tmp = p_in;
        p_in = p_out;
        p_out = tmp;

        // TODO: check again the condition after wait() ???
        barrier.lock();
        barrier_count++;
        if (barrier_count == thread_number) {
            #ifdef PRINT
            (*p_in).print();
            std::cout << std::endl;
            #endif
            // if all thread have complete unlock and notify all
            barrier_count = 0;
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
    // game iteration number
    game_iteration = atoi(argv[3]);
    // parallelism degree
    thread_number = atoi(argv[4]);
    // starting configuration
    auto conf_num = 0;
    if (argc > 5) {
        conf_num = atoi(argv[5]);
    }

    // data structures
    in = new board(rows,cols);
    out = new board(rows,cols);

    switch (conf_num) {
        case 0:
            game_conf::set_random_conf(*in);
            break;
        case 1 :
            game_conf::set_test_conf_1(*in);
            break;
        case 2 :
            game_conf::set_test_conf_2(*in);
            break;
        case 3:
            game_conf::set_test_conf_3(*in);
            break;
        case 4:
            game_conf::set_test_conf_4(*in);
            break;
    }
    #ifdef PRINT
    in.print();
    #endif

    // time start
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // TODO: think about static splitting [particular case es: th_rows<1 ...]
    auto th_rows = rows / thread_number;
    auto remains = rows % thread_number;

    // thread pool
    std::vector<std::thread> tid;
    int start, stop = 0;
    for(auto i=0; i<thread_number; i++) {
        start = stop;
        stop = (remains > 0) ? start + th_rows : start + th_rows -1;
        #ifdef PRINT
        std::cout << "Thread n." << i << " get rows from " << start << " to " << stop << std::endl;
        #endif
        tid.push_back(std::thread(body, start, stop));
        remains--;
        stop++;
    }

    // await termination
    for(int i=0; i<thread_number; i++)
        tid[i].join();

    // time end
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << std::endl;
    std::cout << "game execution time is: " << duration << " milliseconds" << std::endl;
    std::cout << std::endl;

    // data structures clean
    delete in;
    delete out;

    return 0;
}
