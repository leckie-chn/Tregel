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

    WorkerImpl service(argv[1]);
    ServerBuilder builder;
    builder.AddListeningPort(service.GetServiceAddr(), InsecureServerCredentials());
    builder.RegisterService(&service);
    unique_ptr<Server> server(builder.BuildAndStart());

    server->Wait();

    return 0;
}
