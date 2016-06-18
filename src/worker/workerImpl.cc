#include "workerImpl.h"

#include <grpc++/grpc++.h>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>


#include "master.pb.h"

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

using boost::property_tree::ptree;

WorkerImpl::WorkerImpl(const string & initfl) {
        RegisterRequest request;
        RegisterReply reply;
        ClientContext context;

        LoadFromXML(initfl);
        stub_ = MasterService::NewStub(grpc::CreateChannel(mAddr_, grpc::InsecureChannelCredentials()));

        request.set_clientaddr(hAddr_);
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

void WorkerImpl::LoadFromXML(const string & xmlflname) {
    ptree pt;
    read_xml(xmlflname, pt);
    
    string host = pt.get<string>("configure.host");
    string port = pt.get<string>("configure.port");
    hAddr_ = host + ":" + port;
  
    mAddr_ = pt.get<string>("configure.master");
}
