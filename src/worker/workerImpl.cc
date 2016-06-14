#include "workerImpl.h"

#include <grpc++/grpc++.h>
#include <iostream>


#include "../rpc/master.pb.h"

using grpc::ServerContext;
using grpc::Status;
using grpc::ClientContext;
using worker::StartRequest;
using worker::StartReply;
using worker::PushRequest;
using worker::PushReply;
using master::RegisterRequest;
using master::RegisterReply;
using master::MasterService;

using std::string;
using std::unique_ptr;
using std::cout;
using std::endl;


WorkerImpl::WorkerImpl(const string & maddr, const string & me):
    addrme_(me),
    maddr_(maddr),
    stub_(MasterService::NewStub(grpc::CreateChannel(maddr, grpc::InsecureChannelCredentials()))) {
        RegisterRequest request;
        RegisterReply reply;
        ClientContext context;

        request.set_clientaddr(addrme_);
        stub_->Register(&context, request, &reply);

    }

Status WorkerImpl::StartTask(ServerContext *ctxt, const StartRequest *req, StartReply *reply) {
    const auto iter = req->vertexpartition().begin();
    if (iter == req->vertexpartition().end())
        cout << "StartTask: Error No value" << endl;
    else
        cout << "StartTask(" << iter->first << "," << iter->second << ")" << endl;
    return Status::OK;
}

Status WorkerImpl::PushModel(ServerContext *ctxt, const PushRequest *req, PushReply *reply) {
    return Status::OK;
}
