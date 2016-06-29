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
        stub_->Register(&context, request, &reply);
        loadFromDisk(nodes, edges);
    }

Status WorkerImpl::StartTask(ServerContext *ctxt, const StartRequest *req, StartReply *reply) {
    const auto iter = req->vertexpartition().begin();
    if (iter == req->vertexpartition().end())
        cout << "StartTask: Error No value" << endl;
    else
        cout << "StartTask(" << iter->first << "," << iter->second << ")" << endl;
    //stub_ = MasterService::NewStub(grpc::CreateChannel(mAddr_, grpc::InsecureChannelCredentials()));
    int size = 10;
    while (size){
        
    }
    writeToDisk(nodes);
    return Status::OK;
}

Status WorkerImpl::PushModel(ServerContext *ctxt, const PushRequest *req, PushReply *reply) {
    return Status::OK;
}

Status WorkerImpl::InformNewPeer(ServerContext *ctxt, const InformRequest * req, InformReply *reply){
    stubs_[0] = WorkerService::NewStub(grpc::CreateChannel(req->workeraddr() ,grpc::InsecureChannelCredentials()));
    //stubs_[0] = stub;
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

void WorkerImpl::loadFromDisk(map<int, float> & nodes, map<int, int> & edges){
    FILE *fp = fopen("node.txt","r");
    while (!feof(fp)){
        int x;
        float y;
        fscanf(fp,"%d %f",&x, &y);
        nodes[x] = y;
    }
    fclose(fp);
    fp = fopen("graph.txt","r");
    while (!feof(fp)){
        int x,y;
        fscanf(fp,"%d %d",&x, &y);
        edges[x] = y;
    }
    fclose(fp);    
}

void WorkerImpl::writeToDisk(map<int, float> & nodes){
    FILE *fp = fopen("node.txt","w");
    map<int,float>::iterator it;
    for (it = nodes.begin(); it!=nodes.end(); it++)
        fprintf(fp,"%d %f", it->first, it->second);
    fclose(fp);
}
