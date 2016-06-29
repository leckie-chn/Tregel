#include "workerImpl.h"

#include <grpc++/grpc++.h>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>


#include "master.pb.h"
using namespace grpc;
using namespace worker;
using namespace master;
using namespace std;
using namespace std::chrono;

using boost::property_tree::ptree;


WorkerImpl::WorkerC::WorkerC(const string & _addr):
    addr_ (_addr),
    stub_ (WorkerService::NewStub(CreateChannel(_addr, grpc::InsecureChannelCredentials()))) {
    }

WorkerImpl::WorkerImpl(const string & initfl) {
        RegisterRequest request;
        RegisterReply reply;
        ClientContext context;

        LoadFromXML(initfl);
        stub_ = MasterService::NewStub(grpc::CreateChannel(mAddr_, grpc::InsecureChannelCredentials()));

        request.set_clientaddr(hAddr_);
        stub_->Register(&context, request, &reply);
        loadFromDisk(nodes, edges);
    }

Status WorkerImpl::StartTask(ServerContext *ctxt, const StartRequest *req, StartReply *reply_) {
    int sent_cnt = 10;
    while (sent_cnt){
        map<std::string, std::unique_ptr<WorkerC>>::iterator iter;
        for (iter = Workers_.begin(); iter != Workers_.end(); iter++){
            if (!(iter->second)->hasmodel){
                if (pull(*(iter->second.get()))) sent_cnt--;
            }
        }
    }
    page_rank();
    BarrierRequest request;
    BarrierReply reply;
    ClientContext context;
    context.set_deadline(system_clock::time_point(system_clock::now() + seconds(5)));
    request.set_workeraddr(hAddr_);
    stub_->Barrier(&context, request, &reply);
    writeToDisk(nodes);
    
    return Status::OK;
}

Status WorkerImpl::PullModel(ServerContext *ctxt, const PullRequest *req, PullReply *reply_) {
    reply_->clear_model();
    map<int,float>::iterator it;
    for (it = nodes.begin(); it!=nodes.end(); it++){
        (*(reply_->mutable_model()))[it->first] = it->second;
    }
    
    return Status::OK;
}

Status WorkerImpl::InformNewPeer(ServerContext *ctxt, const InformRequest * req, InformReply *reply_){
    Workers_[req->workeraddr()].reset(new WorkerC(req->workeraddr()));
    //stubs_[0] = stub;
    return Status::OK;
}

bool WorkerImpl::pull(WorkerC & c){
    PullRequest request;
    PullReply reply;
    ClientContext context;
    c.stub_->PullModel(&context, request, &reply);
    if (reply.status() == PullReply::OK){
        auto mp = reply.model();
        for (auto it = mp.begin();it != mp.end(); it++){
            int x = it->first;
            float y = it->second;
            nodes[x] = y;
        }
        return true;
    }
    return false;
}

void WorkerImpl::page_rank(){

}

void WorkerImpl::LoadFromXML(const string & xmlflname) {
    ptree pt;
    read_xml(xmlflname, pt);
    
    string host = pt.get<string>("configure.host");
    string port = pt.get<string>("configure.port");
    startid = pt.get<int>("configure.nodestart");
    endid = pt.get<int>("configure.nodeend");
    hAddr_ = host + ":" + port;
  
    mAddr_ = pt.get<string>("configure.master");
}

void WorkerImpl::loadFromDisk(map<int, float> & nodes, std::map<int, std::vector<int>>& edges){
    FILE *fp = fopen("node.txt","r");
    while (!feof(fp)){
        int x;
        float y;
        fscanf(fp,"%d %f",&x, &y);
        nodes[x] = y;
        if ((x >= startid) && (x < endid)){
            local_nodes[x] = y;
        }
    }
    fclose(fp);
    fp = fopen("graph.txt","r");
    while (!feof(fp)){
        int x,y;
        fscanf(fp,"%d %d",&x, &y);
        out_degree[x]++;
        edges[x].push_back(y);
    }
    fclose(fp);    
}

void WorkerImpl::writeToDisk(map<int, float> & nodes){
    FILE *fp = fopen("node.txt","w");
    map<int,float>::iterator it;
    for (it = nodes.begin(); it!=nodes.end(); it++){
        if ((it -> first >= startid) && (it->first < endid))
            fprintf(fp,"%d %f", it->first, it->second);
    }
    fclose(fp);
}
