#include "src/worker/workerImpl.h"

using namespace master;
using namespace grpc;

extern WorkerImpl *impl;

void writeToDisk(){
    impl->version++;
    impl->version_string=to_string(impl->version);
    leveldb::WriteOptions write_options;
    for (int i=impl->startid;i<impl->endid;i++) {
        impl->db->Put(write_options, impl->version_string+"_"+to_string(i), to_string(impl->local_nodes[i]));
    }
    write_options.sync = true;
    impl->db->Put(write_options,"version",impl->version_string);
}

void page_rank(){
    for (int i=impl->startid;i<impl->endid;i++){
        auto v = impl->edges[i];
        float score = 0;
        int tot = v.size();
        for (auto it = v.begin(); it != v.end(); it++){
            int y = *it;
            score += float(impl->nodes[y]) / impl->out_degree[y];
        }
        impl->local_nodes[i] = score;
    }
}

void *compute_thread(void *arg) {

    ClientContext context;
    BarrierRequest request;
    BarrierReply reply;
    
    //TODO: load from disk
    std::string value;
    for (int i=impl->startid;i<impl->endid;i++) {
        impl->db->Get(leveldb::ReadOptions(), impl->version_string+"_"+to_string(i), &value);
        std::stoi(value,impl->local_nodes[i])
    }
    
    request.set_roundno(impl->version);
    request.set_converge(false);
    impl->stub->Barrier(&context, request, &reply);
    for (;;) {
        if (reply.done()) {
            break;// TODO shutdown
        }
        // TODO pull model from peer workers
        page_rank();
        writeToDisk(); //version++
        request.set_roundno(version);
        request.set_converge(false);// TODO judge whether this round of computation has converged
        impl->stub->Barrier(&context, request, &reply);
    }
    return NULL;
}

