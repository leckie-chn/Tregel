#include "masterImpl.h"

#include <iostream>
#include <chrono>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>


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
using boost::property_tree::ptree;

using namespace std;
using namespace std::chrono;


MasterImpl::WorkerC::WorkerC(const string & _addr):
    addr_ (_addr),
    stub_ (WorkerService::NewStub(CreateChannel(_addr, InsecureChannelCredentials()))) {
    }

MasterImpl::MasterImpl(const string &initfl):mtxWorkers_(PTHREAD_MUTEX_INITIALIZER){
    LoadFromXML(initfl);
}

Status MasterImpl::Register(ServerContext *_ctxt,
        const RegisterRequest *_req,
        RegisterReply *_reply) {

    pthread_mutex_lock(&mtxWorkers_);
    const std::string & caddr = _req->clientaddr();
    Workers_[caddr].reset(new WorkerC(caddr));
    if (--unRegWorkerN_ == 0) {
        StartJobs();
    }
    pthread_mutex_unlock(&mtxWorkers_);

    return Status::OK;
}

Status MasterImpl::Barrier(ServerContext *_ctxt,
        const BarrierRequest *_req,
        BarrierReply *_reply) {
    return Status::OK;
}

void MasterImpl::LoadFromXML(const string &xmlname) {
    ptree pt;
    read_xml(xmlname, pt);
    string host = pt.get<string>("configure.host");
    string port = pt.get<string>("configure.port");
    servAddr_ = host + ":" + port;
    BOOST_FOREACH(ptree::value_type &v,
            pt.get_child("configure.workers")) {
        string waddr = v.second.data();
        Workers_[waddr] = nullptr;
    }
    unRegWorkerN_ = Workers_.size();

    return;
}

void MasterImpl::StartJobs() {
    cout << "Register Done" << endl;
    StartRequest request;
    unsigned cnt = 0;

    for (auto & iter : Workers_) {
        auto & worker = iter.second;
        cout << "Invoking Client " << worker->addr_ << endl;
        ClientContext context;
        StartReply reply;

        context.set_deadline(system_clock::time_point(system_clock::now() + seconds(5)));
        (*request.mutable_vertexpartition())[cnt] = cnt;
        cnt++;
        worker->stub_->StartTask(&context, request, &reply);
        request.clear_vertexpartition();
    }

    return;
}
