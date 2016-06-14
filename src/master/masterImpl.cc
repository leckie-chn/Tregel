#include "masterImpl.h"

#include <iostream>

using master::MasterService;
using master::RegisterRequest;
using master::RegisterReply;
using master::BarrierRequest;
using master::BarrierReply;
using worker::WorkerService;
using worker::StartRequest;
using worker::StartReply;
using grpc::ServerContext;
using grpc::Status;
using grpc::Channel;
using grpc::CreateChannel;
using grpc::ClientContext;
using grpc::InsecureChannelCredentials;

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::unique_ptr;

MasterImpl::WorkerC::WorkerC(const string & _addr):
    addr_ (_addr),
    stub_ (WorkerService::NewStub(CreateChannel(_addr, InsecureChannelCredentials()))) {
    }

MasterImpl::MasterImpl(int n):workerN_(n), mtxWorkers_(PTHREAD_MUTEX_INITIALIZER){}

Status MasterImpl::Register(ServerContext *_ctxt,
        const RegisterRequest *_req,
        RegisterReply *_reply) {
    pthread_mutex_lock(&mtxWorkers_);
    Workers_.push_back(unique_ptr<WorkerC>(new WorkerC(_req->clientaddr())));
    if (Workers_.size() == workerN_) {
        // TODO StartJob
        StartJob();
    }
    pthread_mutex_unlock(&mtxWorkers_);

    return Status::OK;
}

Status MasterImpl::Barrier(ServerContext *_ctxt,
        const BarrierRequest *_req,
        BarrierReply *_reply) {
    return Status::OK;
}

void MasterImpl::StartJob() {
    cout << "Register Done" << endl;
    StartRequest request;

    for (int i = 0; i < Workers_.size(); i++) {
        auto & worker = Workers_[i];
        cout << "Invoking Client " << worker->addr_ << endl;
        ClientContext context;
        StartReply reply;
        (*request.mutable_vertexpartition())[i] = i;
        worker->stub_->StartTask(&context, request, &reply);
    }

    return;
}
