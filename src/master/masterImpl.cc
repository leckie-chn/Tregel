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
    stub_ (WorkerService::NewStub(CreateChannel(_addr, InsecureChannelCredentials()))) {
    }

MasterImpl::MasterImpl(const string &confxml):
        mtxWorkers_(PTHREAD_MUTEX_INITIALIZER),
        conf_(confxml),
        servAddr_(conf_.GetMasterAddr()){
    auto workers = conf_.GetWorkerConfs();
    for (auto iter : workers) {
        Workers_.insert(make_pair(iter.first, unique_ptr<WorkerC>()));
    }
}

Status MasterImpl::Register(ServerContext *_ctxt,
        const RegisterRequest *_req,
        RegisterReply *_reply) {

    static int unRegWorkerN = Workers_.size();

    pthread_mutex_lock(&mtxWorkers_);
    const std::string & caddr = _req->clientaddr();
    Workers_[caddr].reset(new WorkerC(caddr));
    if (--unRegWorkerN == 0) {
        // TODO: Start Tasks in an async manner
    }
    pthread_mutex_unlock(&mtxWorkers_);

    return Status::OK;
}

Status MasterImpl::Barrier(ServerContext *_ctxt,
        const BarrierRequest *_req,
        BarrierReply *_reply) {

    cout << "Worker " << _req->workeraddr() << "Round " << _req->roundno() << "\tStart" << endl;

    return Status::OK;
}



