#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "old_board.h"
#include "old_game_logic.h"
#include "old_game_conf.h"

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
old_board * in;
old_board * out;
int game_iteration;

// synchronization lock (some kind of barrier)
std::mutex m;
std::condition_variable cv;

// synchronization variable
int barrier_count = 0;
int thread_number;


void body(int start, int stop) {

    //std::cout << "Thread n. " << std::this_thread::get_id() << " start executing..."<< std::endl;

    const auto col = in->m_width;
    old_board * p_in = in;
    old_board * p_out = out;

    // TODO: this must be in the main (one time initialization) ???
    // synchronization lock (some kind of barrier)
    std::unique_lock<std::mutex> barrier(m, std::defer_lock);

    // game iteration
    for (int k = 0; k < game_iteration; ++k) {

        for (auto i = start; i <= stop; ++i) {
            for (auto j = 0; j < col; ++j) {
                game_logic::update(i,j,*p_in,*p_out);
            }
        }

        // swap pointer
        old_board * tmp = p_in;
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
    in = new old_board(rows, cols);
    out = new old_board(rows, cols);

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
