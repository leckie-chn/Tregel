#include "src/worker/workerImpl.h"

using namespace master;
using namespace grpc;

extern WorkerImpl *impl;

void *compute_thread(void *arg) {
    // TODO recover from failure: rd is not 0 at first
    for (int rd = 0; ; rd++) {
        // TODO pull model from peer workers
        // TODO this round of computation
        // TODO checkpoint
        
        ClientContext context;
        BarrierRequest request;
        BarrierReply reply;

        request.set_roundno(rd);
        request.set_converge(false);    // TODO judge whether this round of computation has converged
        impl->stub_->Barrier(&context, request, &reply);

        if (reply.done()) {
            // TODO shutdown
        }
    }
    return NULL;
}

void page_rank(){
    for (auto it = impl->local_nodes.begin(); it != impl->local_nodes.end(); it++){
        int x = it->first;
        auto v = impl->edges[x];
        float score = 0;
        int tot = v.size();
        for (auto it = v.begin(); it != v.end(); it++){
            int y = *it;
            score += float(impl->nodes[y]) / impl->out_degree[y];
        }
        impl->local_nodes[x] = score;
    }
}
