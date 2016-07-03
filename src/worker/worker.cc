#include <iostream>
#include <string>
#include <grpc++/grpc++.h>
#include <pthread.h>

#include "master.grpc.pb.h"
#include "workerImpl.h"


using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Server;
using grpc::Status;
using grpc::InsecureServerCredentials;

using std::string;
using std::unique_ptr;

WorkerImpl *impl;
pthread_t pid;

int main(int argc, char **argv) {

    WorkerImpl service(argv[1]);
    impl = &service;
    ServerBuilder builder;
    builder.AddListeningPort(service.GetServiceAddr(), InsecureServerCredentials());
    builder.RegisterService(&service);
    unique_ptr<Server> server(builder.BuildAndStart());

    pthread_create(&pid, NULL, compute_thread, NULL);
    server->Wait();
    pthread_join(pid, NULL);

    return 0;
}
