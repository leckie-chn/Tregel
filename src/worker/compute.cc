#include "src/worker/workerImpl.h"
#include "src/util/logger.h"
#include <unistd.h>
#include <math.h>
#include <grpc++/grpc++.h>

using namespace master;
using namespace grpc;
using namespace std;
using namespace worker;

extern WorkerImpl *impl;
extern unique_ptr<Server> server;

bool pull(WorkerImpl::WorkerC & c) {
    PullRequest request;
    PullReply reply;
    ClientContext context;
    request.set_roundno(impl->version);
    Status status = c.stub->PullModel(&context, request, &reply);
    if (status.ok()){
        //if (reply.status() == PullReply::OK){
        auto mp = reply.model();
        for (auto it = mp.begin();it != mp.end(); it++){
            int x = it->first;
            float y = it->second;
            impl->nodes[x] = y;
        }
        return true;
    }
    return false;
}

void page_rank(){
    for (int i=impl->startid;i<impl->endid;i++){
        auto v = impl->edges[i];
        float score = 0;
        int tot = v.size();
        for (auto it = v.begin(); it != v.end(); it++){
            int y = *it;
            score += impl->nodes[y] / impl->out_degree[y];
        }
        impl->local_nodes[i] = score;
    }
}

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

void *compute_thread(void *arg) {


    //TODO: load from disk
    std::string value;
    for (int i=impl->startid;i<impl->endid;i++) {
        impl->db->Get(leveldb::ReadOptions(), impl->version_string+"_"+to_string(i), &value);
        impl->local_nodes[i]=std::stof(value);
    }


    for (;;) {
        ClientContext context;
        BarrierRequest request;
        BarrierReply reply;
        
        request.set_roundno(impl->version);
        request.set_workeraddr(impl->GetServiceAddr());
        request.set_converge(impl->converged);
        
        impl->stub->Barrier(&context, request, &reply);
        if (reply.done()) {
            FILE * fp = fopen("out.txt","w");
            for (int i=impl->startid;i<impl->endid;i++) {
                fprintf(fp, "%d %f\n", i,impl->local_nodes[i]);
            }
            fclose(fp);
            LOG("shutdown on Round %d\n", impl->version);           
            server->Shutdown();
            break; 
        }
        int workmate_count=0;
        for (auto iter = impl->Workers.begin(); iter != impl->Workers.end(); iter++){
            iter->second.hasmodel=false;
            workmate_count++;
        }
        while (workmate_count) {
            for (auto iter = impl->Workers.begin(); iter != impl->Workers.end(); iter++){
                if (!iter->second.hasmodel){
                    if (iter->second.hasmodel=pull(iter->second)) workmate_count--;
                }
            }
        }
        //load local model
        for (int i=impl->startid;i<impl->endid;i++) {
            impl->nodes[i]=impl->local_nodes[i];
        }
        page_rank();
        impl->converged = true;
        for (int i=impl->startid;i<impl->endid;i++) {
            if (fabs(impl->nodes[i]-impl->local_nodes[i]) > 1e-2){
                impl->converged = false;
            }
        }        
        writeToDisk(); //version++
        request.set_roundno(impl->version);
        // TODO judge whether this round of computation has converged
        LOG("Round %d done\n", impl->version);
    }
    return NULL;
}

