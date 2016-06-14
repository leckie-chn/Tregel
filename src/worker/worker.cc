#include <iostream>
#include <string>
#include <grpc++/grpc++.h>

#include "../rpc/master.grpc.pb.h"
#include "workerImpl.h"

using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Server;
using grpc::Status;
using grpc::InsecureServerCredentials;

using std::string;
using std::unique_ptr;


int main(int argc, char **argv) {

    string master_port(argv[1]);
    string worker_port(argv[2]);
    string masteraddr = "162.105.146.42:" + master_port;
    string workeraddr = "162.105.146.42:" + worker_port;
    
    WorkerImpl service(masteraddr, workeraddr);
    ServerBuilder builder;
    builder.AddListeningPort(workeraddr, InsecureServerCredentials());
    builder.RegisterService(workeraddr, &service);
    unique_ptr<Server> server(builder.BuildAndStart());

    server->Wait();

    return 0;
}
