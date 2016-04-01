#include <iostream>
#include <pthread.h>
#include <chrono>

#include "board.h"
#include "game_conf.h"

/*
 *
 *
 * compile with: g++ pthread.cpp -std=c++11 -O3 -pthread -o pthread.exe
 * run with: ./threads.exe @row_number @colum_number @iteration_number @parallelism degree [@configuration_number (from 1 to 4)]
 *
 *
 * */

// #define PRINT

struct thread_data{
    int _start;
    int _stop;
};

// synchronization lock (some kind of barrier)
pthread_barrier_t barrier;

// TODO: probably heap pointers are not a good idea
// game boards
board* in;
board* out;

int iter_num;

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

void* body(void* arg) {

    thread_data* data = (thread_data*) arg;
    board* p_in = in;
    board* p_out = out;
    int start = data->_start;
    int stop = data->_stop;

    // columns number
    const auto col = p_in->m_width;

    // game iteration
    for (int k = 0; k < iter_num; ++k) {

        for (auto i = start; i <= stop; ++i) {
            for (auto j = 0; j < col; ++j) {
                update(i,j,*p_in,*p_out);
            }
        }

        // swap pointer
        board* tmp = p_in;
        p_in = p_out;
        p_out = tmp;

        int res = pthread_barrier_wait(&barrier);
        if(res == PTHREAD_BARRIER_SERIAL_THREAD) {
            #ifdef PRINT
            (*p_in).print();
            #endif
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
    // starting configuration
    auto conf_num = 0;
    if (argc > 5) {
        conf_num = atoi(argv[5]);
    }

    // TODO: use or not "new" ???
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
