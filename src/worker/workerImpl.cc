#include "workerImpl.h"

#include <grpc++/grpc++.h>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


#include "master.pb.h"

using namespace grpc;
using namespace worker;
using namespace master;
using namespace std;

using boost::property_tree::ptree;

WorkerImpl::WorkerImpl(const string & initfl) {
        RegisterRequest request;
        RegisterReply reply;
        ClientContext context;

        LoadFromXML(initfl);
        mStub_ = MasterService::NewStub(CreateChannel(mAddr_, InsecureChannelCredentials()));

        request.set_clientaddr(hAddr_);
        mStub_->Register(&context, request, &reply);

    }

Status WorkerImpl::StartTask(ServerContext *_ctxt, const StartRequest *_req, StartReply *_reply) {
    for (auto iter = _req->vertexpartition().begin(); iter != _req->vertexpartition().end(); iter++)
        if (iter->first == hAddr_) {
            // this worker
            vertexb_ = iter->second;
        } else {
            // other workers
            prStubs_[iter->first] = WorkerService::NewStub(CreateChannel(iter->first, InsecureChannelCredentials()));
        }

    ClientContext context;
    BarrierRequest request;
    BarrierReply reply;
    request.set_workeraddr(hAddr_);
    request.set_roundno(0);
    mStub_->Barrier(&context, request, &reply);

    return Status::OK;
}

Status WorkerImpl::PushModel(ServerContext *ctxt, const PushRequest *req, PushReply *reply) {
    return Status::OK;
}

void WorkerImpl::LoadFromXML(const string & xmlflname) {
    ptree pt;
    read_xml(xmlflname, pt);
    
    string host = pt.get<string>("configure.host");
    string port = pt.get<string>("configure.port");
    hAddr_ = host + ":" + port;
  
    mAddr_ = pt.get<string>("configure.master");
}