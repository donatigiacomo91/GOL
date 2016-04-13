#include <iostream>
#include <vector>
#include <unistd.h>

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>

#include "board.h"

using namespace ff;

struct task {
    int start;
    int stop;
};

struct Worker: ff_node_t<task> {

    task* svc(task* t) {

        usleep(get_my_id()*1000 + (rand()%5)*10000 );
        std::cout << " get." << t->start << "," << t->stop <<std::endl;

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
            task_arr[i]  = {start, stop};
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
            std::cout << "iteration completed\n";
            iteration_count++;
            worker_count = 0;

            if (iteration_count == iteration_num) {
                std::cout << "game completed\n";
                lb->broadcast_task(EOS);
                free(task_arr);
                return EOS;
            } else {
                for(int i=0; i < worker_num; ++i)
                    ff_send_out(&task_arr[i]); // round robin strategy
                    //lb->ff_send_out_to(new long(i),i);
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
    in->print();
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
