#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "board.h"

/*
 *
 *
 * compile with: g++ threads.cpp -std=c++11 -O3 -o threads.exe
 * run with: ./threads.exe @row_number @colum_number @iteration_number @parallelism degree [@configuration_number (from 1 to 4)]
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

    std::cout << "Thread n. " << std::this_thread::get_id() << " start executing..."<< std::endl;

    // columns number
    const auto col = p_in->m_width;
    // TODO: this must be in the main (one time initialization) ???
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
            (*p_in).print();
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

void set_random_conf(board& b) {
    for(auto i=0; i<b.m_width; i++)
        for(auto j=0; j<b.m_height; j++)
            b[i][j] = rand()%2;
}

// Beacon test conf (periodic)
void set_test_conf_1(board& b) {
    b[0][0] = 1;
    b[0][1] = 1;
    b[1][0] = 1;
    b[1][1] = 1;

    b[2][2] = 1;
    b[2][3] = 1;
    b[3][2] = 1;
    b[3][3] = 1;
}

// Blinker test conf (periodic)
void set_test_conf_2(board& b) {
    b[b.m_height/2][b.m_width/2] = 1;
    b[b.m_height/2][b.m_width/2+1] = 1;
    b[b.m_height/2][b.m_width/2-1] = 1;
}

// Glider test conf (dynamic)
// note: in a 10x10 matrix come back to initial conf in 40 iterations
void set_test_conf_3(board& b) {
    b[0][1] = 1;
    b[1][2] = 1;
    b[2][0] = 1;
    b[2][1] = 1;
    b[2][2] = 1;
}

// Beehive test conf (static)
void set_test_conf_4(board& b) {
    b[0][1] = 1;
    b[0][2] = 1;
    b[1][0] = 1;
    b[1][3] = 1;
    b[2][1] = 1;
    b[2][2] = 1;
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

    auto conf_num = atoi(argv[5]);

    // data structures
    board in(rows,cols);
    board out(rows,cols);

    switch (conf_num) {
        case 0:
            set_random_conf(in);
            break;
        case 1 :
            set_test_conf_1(in);
            break;
        case 2 :
            set_test_conf_2(in);
            break;
        case 3:
            set_test_conf_3(in);
            break;
        case 4:
            set_test_conf_4(in);
            break;
    }
    in.print();

    // TODO: think about static splitting [particular case es: th_rows<1 ...]

    auto th_rows = rows / th_num;
    auto remains = rows % th_num;

    // thread pool
    std::vector<std::thread> tid;
    int start, stop = 0;
    for(auto i=0; i<th_num; i++) {
        start = stop;
        stop = (remains > 0) ? start + th_rows : start + th_rows -1;
        std::cout << "Thread n." << i << " get rows from " << start << " to " << stop << std::endl;
        tid.push_back(std::thread(body, start, stop, it_num, &in, &out));
        remains--;
        stop++;
    }

    // await termination
    for(int i=0; i<th_num; i++)
        tid[i].join();

    return 0;
}
