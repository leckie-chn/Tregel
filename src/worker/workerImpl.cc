#include "workerImpl.h"

#include <grpc++/grpc++.h>
#include <iostream>
#include <unistd.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include "src/util/XmlLoader.h"

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

    leveldb::Status s = db->Get(leveldb::ReadOptions(), "version", &version_string);
    if (s.ok()) {
        version=std::stoi(version_string);
    } else { //init db
        version=0;
        version_string="0";
        leveldb::WriteOptions write_options;
        for (int i=startid;i<endid;i++) {
            db->Put(write_options, "0_"+to_string(i), "0.5"); //TODO:0.5?
        }
        write_options.sync = true;
        db->Put(write_options,"version",version_string);
    }
    FILE *fp = fopen(graphpath.c_str(),"r");
    while (!feof(fp)){
        int x,y;
        fscanf(fp,"%d %d",&x, &y);
        out_degree[x]++;
        edges[x].push_back(y);
    }
    fclose(fp);
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

    startid = pt.get<int>("configure.nodestart");
    endid = pt.get<int>("configure.nodeend");
    hAddr = pt.get<string>("configure.host");

    mAddr = pt.get<string>("configure.master");
    
    graphpath = pt.get<string>("configure.graphpath");
    string dbpath = pt.get<string>("configure.dbpath");
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, dbpath, &db);
    assert(status.ok());
    
    auto workers = pt.GetWorkerVec();
    for (std::string iter : workers) {
        if (iter != hAddr) Workers.insert(make_pair(iter, new WorkerC(iter)));
    }

}

// void WorkerImpl::loadFromDisk(){
    // FILE *fp = fopen("node.txt","r");
    // while (!feof(fp)){
        // int x;
        // float y;
        // fscanf(fp,"%d %f",&x, &y);
        // local_nodes[x] = y;
    // }
    // fclose(fp);
    // fp = fopen(graphpath.c_str(),"r");
    // while (!feof(fp)){
        // int x,y;
        // fscanf(fp,"%d %d",&x, &y);
        // out_degree[x]++;
        // edges[x].push_back(y);
    // }
    // fclose(fp);
// }


// void WorkerImpl::writeToDisk(){
    // version++;
    // string version_string=to_string(version);
    // leveldb::WriteOptions write_options;
    // for (int i=startid;i<endid;i++) {
        // db->Put(write_options, version_string+"_"+to_string(i), to_string(local_nodes[i]));
    // }
    // write_options.sync = true;
    // db->Put(write_options,"version",version_string);
// }
