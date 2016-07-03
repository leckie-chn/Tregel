#include "masterImpl.h"

#include <iostream>
#include <chrono>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>



using boost::property_tree::ptree;

using namespace std;
using namespace std::chrono;
using namespace grpc;
using namespace worker;
using namespace master;

MasterImpl::WorkerC::WorkerC():
    waiting_(false),
    converge_(false) {}

MasterImpl::MasterImpl(const string &confxml):
    conf_(confxml),
    roundno_(-1),
    mu_(PTHREAD_MUTEX_INITIALIZER),
    cond_(PTHREAD_COND_INITIALIZER),
    servAddr_(conf_.GetMasterAddr()){
        auto workers = conf_.GetWorkerConfs();
        for (auto iter : workers) {
            Workers_.insert(make_pair(iter.first, WorkerC()));
        }
    }


Status MasterImpl::Barrier(ServerContext *_ctxt,
        const BarrierRequest *_req,
        BarrierReply *_reply) {

    int round = _req->roundno();
    
    if (round <= roundno_) {
        // rare case, but MAY happen when a worker recover from failure
        // do not block, return OK
        _reply->set_status(BarrierReply_BarrierStatus_OK);
        _reply->set_done(false);
    } else {
        // Worker wait for synchronization, normal case
        pthread_mutex_lock(&mu_);
        WorkerC & worker = Workers_[_req->workeraddr()];
        worker.waiting_ = true;
        worker.converge_ = _req->converge();
    
        bool allsync = true;
        for (auto & iter : Workers_)
            if (!iter.second.waiting_) {
                allsync = false;
                break;
            } 

        if (allsync) {
            halt_ = haltRound();
            roundno_++;
            pthread_cond_broadcast(&cond_);
        } else 
            pthread_cond_wait(&cond_, &mu_);
        
        pthread_mutex_unlock(&mu_);
    } 

    return Status::OK;
}

bool MasterImpl::haltRound() {
    for (auto & iter : Workers_) 
        if (!iter.second.converge_)
            return false;
    return true;
}
