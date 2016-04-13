#include <iostream>
#include <vector>
#include <unistd.h>

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>

#include "board.h"

using namespace ff;

#define PRINT

struct task {
    int start;
    int stop;
    board* _p_in;
    board* _p_out;
};

struct Worker: ff_node_t<task> {

    task* svc(task* t) {
        board * p_in = t->_p_in;
        board * p_out = t->_p_out;
        int start = t->start;
        int stop = t->stop;

        // columns number
        const auto cols = p_in->m_width;
        const auto rows = p_in->m_height;

        int* matrix_in = p_in->matrix;
        int* matrix_out = p_out->matrix;

        const auto assigned_row_num = (stop-start+1);

        // current, upper and lower indices
        auto up_p = start*cols + 1;
        auto curr_p = up_p + cols;
        auto low_p = curr_p + cols;

        #pragma ivdep
        for (int i = 1; i < assigned_row_num*cols; ++i) {
            std::cout << up_p << "," << curr_p << "," << low_p << std::endl;
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
	    std::cout << "main loop end" << std::endl;

        // set left and right border
        int left = start*cols + cols;
        int right = left+cols - 1;
        // no vectorization here (noncontiguous memory access make it inefficient)
        for (int i = 0; i < assigned_row_num; i++) {
            matrix_out[left] = matrix_out[right-1];
            matrix_out[right] = matrix_out[left+1];
            left += cols;
            right += cols;
        }

        // thread that compute the first row have to copy it as bottom border
        if (start == 0) {
            const auto sr_index = cols;
            const auto bb_index = (rows-1)*cols;
            for (int i = 0; i < cols; ++i) {
                matrix_out[bb_index+i] = matrix_out[sr_index+i];
            }
        }
        // thread that compute the last row have to copy it as upper border
        if (stop == rows-3) {
            const auto slr_index = (rows-2)*cols;
            for (int ub_index = 0; ub_index < cols; ++ub_index) {
                matrix_out[ub_index] = matrix_out[slr_index+ub_index];
            }
        }

        return t;
    }

};

struct Emitter: ff_node_t<task> {

    int iteration_num;
    int iteration_count;

    int worker_num;
    int worker_count;

    ff_loadbalancer *lb;
    board* p_in;
    board* p_out;

    task* task_arr;

    Emitter(int iteration, int worker, ff_loadbalancer *const farm_lb, board* in, board* out) {

        iteration_num = iteration;
        iteration_count = 0;
        worker_num = worker;
        worker_count = 0;
        lb = farm_lb;
        p_in = in;
        p_out = out;

        // build workers partions
        task_arr = (task*) malloc(sizeof(task)*worker);

        auto th_rows = (in->m_height-2) / worker;
        auto remains = (in->m_height-2) % worker;
        int start, stop = 0;
        for(auto i=0; i<worker; i++) {
            start = stop;
            stop = (remains > 0) ? start + th_rows : start + th_rows -1;
            task_arr[i]  = {start, stop, in, out};
            remains--;
            stop++;
        }
    }

    int svc_init() {
        std::cout << "Emitter init\n";

        for(int i=0; i < worker_num; ++i) {
            ff_send_out(&task_arr[i]);
        }

	    std::cout << "Emitter init end\n";
        return 0;
    }

    task *svc(task *t) {

        worker_count++;

        if (worker_count == worker_num) {

            iteration_count++;
            worker_count = 0;
            #ifdef PRINT
            t->_p_out->print();
            std::cout << std::endl;
            #endif

            if (iteration_count == iteration_num) {
                std::cout << "game completed\n";
                lb->broadcast_task(EOS);
                free(task_arr);
                return EOS;
            } else {
                for(int i=0; i < worker_num; ++i) {
                    // switch pointer
                    board* tmp = task_arr[i]._p_out;
                    task_arr[i]._p_out = task_arr[i]._p_in;
                    task_arr[i]._p_in = tmp;
                    ff_send_out(&task_arr[i]); // round robin strategy
                    // lb->ff_send_out_to(new long(i),i); // static scheduling strategy
                }
            }
        }

        return GO_ON;
    }
};

int main(int argc, char *argv[]) {

    // board size
    auto rows = atoi(argv[1]);
    auto cols = atoi(argv[2]);
    // iteration number
    auto iter_num = atoi(argv[3]);
    // parallelism degree
    auto th_num = atoi(argv[4]);

    // data structures
    board in(rows, cols);
    board out(rows, cols);
    in.set_random();

    #ifdef PRINT
    in.print();
    #endif

    // time start
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // fastflow setup
    std::vector<std::unique_ptr<ff_node>> workers;
    for(int i=0; i<th_num; ++i)
        workers.push_back(make_unique<Worker>());

    ff_Farm<task> farm(std::move(workers));

    Emitter emitter(iter_num, th_num, farm.getlb(), &in, &out);
    // add specific emitter
    farm.add_emitter(emitter);
    // adds feedback channels between each worker and the emitter
    farm.wrap_around();

    if (farm.run_and_wait_end()<0) error("running farm");

    // time end
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << std::endl;
    std::cout << "game execution time is: " << duration << " milliseconds" << std::endl;
    std::cout << std::endl;

    return 0;
}
