#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <chrono>

#include "board.h"

/*
 * local compile with: g++-5 pthread.cpp -std=c++11 -O3 -pthread -o pthread.exe
 */

// #define PRINT

struct thread_data{
    int _start;
    int _stop;
};

// synchronization lock (some kind of barrier)
pthread_barrier_t barrier;

// TODO: probably heap pointers are not a good idea
// game boards
board * in;
board * out;

int iter_num;

void* body(void* arg) {

    thread_data* data = (thread_data*) arg;
    board * p_in = in;
    board * p_out = out;
    int start = data->_start;
    int stop = data->_stop;

    // columns number
    const auto col = p_in->m_width;

    // game iteration
    auto assing = 1;
    for (int k = 0; k < iter_num; ++k) {

        for (auto i = start; i <= stop; ++i) {
            for (auto j = 0; j < col; ++j) {
                assing += assing*(j+i);
            }
        }

//        // swap pointer
//        board * tmp = p_in;
//        p_in = p_out;
//        p_out = tmp;

        std::cout << "th complete" << std::endl;
        int res = pthread_barrier_wait(&barrier);
        if(res == PTHREAD_BARRIER_SERIAL_THREAD) {
            usleep(100000);
            std::cout << "serial th." << std::endl;
        } else if(res != 0) {
            // error occurred
            std::cout << "ERROR in barrier n." << res << std::endl;
        }

    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    // board size
    auto rows = atoi(argv[1]);
    auto cols = atoi(argv[2]);
    // iteration number
    iter_num = atoi(argv[3]);
    // parallelism degree
    auto th_num = atoi(argv[4]);

    // TODO: use or not "new" ???
    // data structures
    in = new board(rows, cols);
    out = new board(rows, cols);

    in->set_random();

    #ifdef PRINT
    in->print();
    #endif

    // time start
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // TODO: think about static splitting [particular case es: th_rows<1 ...]
    auto th_rows = rows / th_num;
    auto remains = rows % th_num;

    // thread pool setup
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // initialize a barrier
    pthread_barrier_init(&barrier, NULL, th_num);

    int start, stop = 0;
    thread_data* t_data = (thread_data*) malloc(sizeof(t_data)*th_num);
    for(auto i=0; i<th_num; i++) {
        start = stop;
        stop = (remains > 0) ? start + th_rows : start + th_rows -1;
        t_data[i]  = {start, stop};
        #ifdef PRINT
        std::cout << "Thread n." << i << " get rows from " << start << " to " << stop << std::endl;
        #endif
        remains--;
        stop++;
    }

    pthread_t* tid = (pthread_t*) malloc(sizeof(pthread_t)*th_num);
    for(auto i=0; i<th_num; i++) {
        auto rc = pthread_create(&tid[i], NULL, body, (void *)&t_data[i]);
        if (rc){
            std::cout << "ERROR; return code from pthread_create() is " << rc << std::endl;
        }
    }

    // await termination
    void *status;
    for(auto i=0; i<th_num; i++) {
        auto rc = pthread_join(tid[i], &status);
        if (rc) {
            std::cout << "ERROR; return code from pthread_join() is " << rc << std::endl;
        }
    }

    // clean up
    pthread_attr_destroy(&attr);
    pthread_barrier_destroy(&barrier);

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