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


MasterImpl::WorkerC::WorkerC(const string & _addr):
    addr_ (_addr),
    roundbarrier_(false),
    stub_ (WorkerService::NewStub(CreateChannel(_addr, InsecureChannelCredentials()))) {
    }

MasterImpl::MasterImpl(const string &confxml):
    mtxWorkers_(PTHREAD_MUTEX_INITIALIZER),
    synclk_(syncmtx_),
    conf_(confxml),
    roundno_(0),
    servAddr_(conf_.GetMasterAddr()){
        auto workers = conf_.GetWorkerConfs();
        for (auto iter : workers) {
            Workers_.insert(make_pair(iter.first, unique_ptr<WorkerC>()));
        }
    }

Status MasterImpl::Register(ServerContext *_ctxt,
        const RegisterRequest *_req,
        RegisterReply *_reply) {

    pthread_mutex_lock(&mtxWorkers_);
    static int unRegWorkerN = Workers_.size();
    const std::string & caddr = _req->clientaddr();
    Workers_[caddr].reset(new WorkerC(caddr));
    if (--unRegWorkerN == 0) {
        startJob();
    }
    pthread_mutex_unlock(&mtxWorkers_);

    return Status::OK;
}

Status MasterImpl::Barrier(ServerContext *_ctxt,
        const BarrierRequest *_req,
        BarrierReply *_reply) {

    if (_req->roundno() == 0) {
        _reply->set_done(true);
        _reply->set_status(BarrierReply_BarrierStatus_OK);
        return Status::OK;
    }

    // Sync Workers on Barrier
    bool allsync = true;
    synclk_.lock();
    Workers_[_req->workeraddr()]->roundbarrier_ = true;
    for (auto &iter : Workers_)
        if (!iter.second->roundbarrier_) {
            allsync = false;
            break;
        }

    // Compute halt_;
    if (roundno_ < _req->roundno()) {
        roundno_ = _req->roundno();
        halt_ = true;
    }
    halt_ &= _req->converge();

    if (allsync) {
        for (auto &iter : Workers_)
            iter.second->roundbarrier_ = false;
        synccond_.notify_all();
    } else
        synccond_.wait(synclk_);

    synclk_.unlock();

    _reply->set_done(halt_);
    _reply->set_status(BarrierReply_BarrierStatus_OK);

    return Status::OK;
}

void MasterImpl::startJob() {
    StartRequest request;
    for (auto &iter : Workers_) {
        auto &wcptr = iter.second;
        wcptr->taskrpc_ = wcptr->stub_->AsyncStartTask(&wcptr->taskcontext_, request, &jobcq_);
        wcptr->taskrpc_->Finish(&wcptr->taskreply_, &wcptr->taskstat_, (void*)1);
    }
}

void MasterImpl::stopJob() {
    int stopcnt = Workers_.size();
    while (stopcnt > 0) {
        void *tag;
        bool ok = false;
        jobcq_.Next(&tag, &ok);
        if (ok)
            if (tag == (void *)1) {
                stopcnt--;
            }
            else {
                // TODO Something Wrong with Workers
            }
    }

}
