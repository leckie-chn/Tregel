#include <iostream>

#include <grpc++/grpc++.h>

#include "../rpc/master.grpc.pb.h"
#include "../rpc/master.pb.h"
#include "masterImpl.h"

using grpc::Status;
using grpc::ServerContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::InsecureServerCredentials;
using grpc::string;


int main(int argc, char **argv) {

    MasterImpl service(10);
    ServerBuilder builder;
    const string ListeningAddr = "0.0.0.0:" + string(argv[1]);
    builder.AddListeningPort(ListeningAddr, InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    server->Wait();

    return 0;
}
