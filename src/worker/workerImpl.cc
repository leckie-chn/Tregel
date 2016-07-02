#include "workerImpl.h"

#include <grpc++/grpc++.h>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <cassert>
#include "leveldb/db.h"

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
    for (auto it = local_nodes.begin(); it!=local_nodes.end(); it++){
        int x = it->first;
        int y = it->second;
        if ((x >= startid) && (x < endid)){
            nodes[x] = y;
        }
    }
    while (true){
        int sent_cnt = 10;
        while (sent_cnt){
            for (auto iter = Workers_.begin(); iter != Workers_.end(); iter++){
                if (!(iter->second)->hasmodel){
                    if (pull(*(iter->second.get()))) {
                        (iter->second)->hasmodel = true;
                        sent_cnt--;
                    }
                }
            }
        }
        page_rank();
        BarrierRequest request;
        BarrierReply reply;
        ClientContext context;
        context.set_deadline(system_clock::time_point(system_clock::now() + seconds(5)));
        request.set_workeraddr(hAddr_);
        writeToDisk(nodes);
        stub_->Barrier(&context, request, &reply);    
    }
    return Status::OK;
}

Status WorkerImpl::PullModel(ServerContext *ctxt, const PullRequest *req, PullReply *reply_) {
    reply_->clear_model();
    for (auto it = nodes.begin(); it!=nodes.end(); it++){
        (*(reply_->mutable_model()))[it->first] = it->second;
    }
    reply_->set_status(PullReply::OK);
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
    Status status = c.stub_->PullModel(&context, request, &reply);
    if (status.ok()){
    //if (reply.status() == PullReply::OK){
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
    for (auto it = local_nodes.begin(); it != local_nodes.end(); it++){
        int x = it->first;
        auto v = edges[x];
        float score = 0;
        int tot = v.size();
        for (auto it = v.begin(); it != v.end(); it++){
            int y = *it;
            score += float(nodes[y])/out_degree[y];
        }
        local_nodes[x] = score;
    }
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
        local_nodes[x] = y;
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
    for (auto it = local_nodes.begin(); it!=local_nodes.end(); it++){
        if ((it -> first >= startid) && (it->first < endid))
            fprintf(fp,"%d %f", it->first, it->second);
    }
    fclose(fp);
}
