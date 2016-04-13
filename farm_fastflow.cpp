#include <iostream>
#include <vector>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>

using namespace ff;

struct Worker: ff_node_t<long> {
    long *svc(long *t) {
        std::cout << *t << std::endl;
        return t;
    } // it does nothing, just sends back tasks
};

struct Emitter: ff_node_t<long> {
    int iteration;
    int iteration_count;
    int worker_num;
    int worker_count;

    ff_loadbalancer *lb;

    Emitter(int iter, int worker, ff_loadbalancer *const lb) {
        iteration = iter;
        iteration_count = 0;
        worker_num = worker;
        worker_count = 0;
    }

    int svc_init() {
        std::cout << "Emitter init\n";
        for(int i=0; i < worker_num; ++i)
            ff_send_out(new long(i));
        return 0;
    }

    long *svc(long *t) {

        delete t;
        worker_count++;

        if (worker_count == worker_num) {
            iteration_count++;
            worker_count = 0;
            if (iteration_count == iteration) {
                lb->broadcast_task(EOS);
            } else {
                for(int i=0; i < worker_num; ++i)
                    ff_send_out(new long(i));
                    //lb->ff_send_out_to(new long(i),i);
            }
        }

        return GO_ON;
    }
};

int main(int argc, char *argv[]) {

    assert(argc>1);

    int game_iteration = atoi(argv[1]);
    int nworkers = atoi(argv[2]);

    std::vector<std::unique_ptr<ff_node>> workers;
    for(int i=0; i<nworkers; ++i)
        workers.push_back(make_unique<Worker>());

    ff_Farm<long> farm(std::move(workers));

    Emitter emitter(game_iteration, nworkers, farm.getlb());
    // add specific emitter
    farm.add_emitter(emitter);
    // adds feedback channels between each worker and the emitter
    farm.wrap_around();

    if (farm.run_and_wait_end()<0) error("running farm");

    return 0;
}