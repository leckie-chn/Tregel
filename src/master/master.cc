#include <iostream>

#include <grpc++/grpc++.h>

#include "master.grpc.pb.h"
#include "master.pb.h"
#include "masterImpl.h"

using grpc::Status;
using grpc::ServerContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::InsecureServerCredentials;
using grpc::string;


int main(int argc, char **argv) {

    // load configure file from xml
    MasterImpl service(argv[1]);
    ServerBuilder builder;
    builder.AddListeningPort(service.GetServiceAddr(), InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    server->Wait();

    return 0;
}
