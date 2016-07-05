#include <iostream>

#include <grpc++/grpc++.h>

#include "master.grpc.pb.h"
#include "master.pb.h"
#include "masterImpl.h"

using namespace grpc;
using namespace std;

unique_ptr<Server> server;

int main(int argc, char **argv) {

    // load configure file from xml
    MasterImpl service(argv[1]);
    ServerBuilder builder;
    builder.AddListeningPort(service.GetServiceAddr(), InsecureServerCredentials());
    builder.RegisterService(&service);
    server = builder.BuildAndStart();
    server->Wait();

    return 0;
}
