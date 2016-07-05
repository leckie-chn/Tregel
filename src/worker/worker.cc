#include <iostream>
#include <string>
#include <grpc++/grpc++.h>
#include <pthread.h>

#include <leveldb/db.h>

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
unique_ptr<Server> server;

int main(int argc, char **argv) {

    impl = new WorkerImpl(argv[1]);
    ServerBuilder builder;
    builder.AddListeningPort(impl->GetServiceAddr(), InsecureServerCredentials());
    builder.RegisterService(impl);
    server = builder.BuildAndStart();

    pthread_create(&pid, NULL, compute_thread, NULL);
    server->Wait();
    pthread_join(pid, NULL);

    return 0;
}
