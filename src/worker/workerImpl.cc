#include "workerImpl.h"

#include <grpc++/grpc++.h>
#include <iostream>
#include <unistd.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <cassert>
#include "leveldb/db.h"

#include "master.pb.h"

#define DEBUG
using namespace grpc;
using namespace worker;
using namespace master;
using namespace std;
using namespace std::chrono;

using boost::property_tree::ptree;


WorkerImpl::WorkerC::WorkerC(const string & _addr):
    addr_ (_addr),
    stub (WorkerService::NewStub(CreateChannel(_addr, grpc::InsecureChannelCredentials()))) {
    }

WorkerImpl::WorkerImpl(const string & initfl) {
    LoadFromXML(initfl);
    stub = MasterService::NewStub(grpc::CreateChannel(mAddr, grpc::InsecureChannelCredentials()));
    // TODO: deal with recover from failure
}


Status WorkerImpl::PullModel(ServerContext *ctxt, const PullRequest *req, PullReply *reply) {
    reply->clear_model();
    for (auto it = local_nodes.begin(); it!=local_nodes.end(); it++){
        (*(reply->mutable_model()))[it->first] = it->second;
    }
    reply->set_status(PullReply::OK);
    return Status::OK;
}

bool WorkerImpl::pull(WorkerC & c) {
    PullRequest request;
    PullReply reply;
    ClientContext context;
    Status status = c.stub->PullModel(&context, request, &reply);
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

    void WorkerImpl::LoadFromXML(const string & xmlflname) {
        ptree pt;
        read_xml(xmlflname, pt);

        string host = pt.get<string>("configure.host");
        string port = pt.get<string>("configure.port");
        startid = pt.get<int>("configure.nodestart");
        endid = pt.get<int>("configure.nodeend");
        hAddr = host + ":" + port;

        mAddr = pt.get<string>("configure.master");
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
