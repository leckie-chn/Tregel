#include <iostream>

#include <grpc++/grpc++.h>

#include "../rpc/master.grpc.pb.h"
#include "../rpc/master.pb.h"

using master::RegisterRequest;
using master::RegisterResponse;
using master::MasterService;
using grpc::Status;
using grpc::ServerContext;
using grpc::Server;
using grpc::ServerBuilder;

class MasterImpl final : public MasterService::Service {
    Status Register(ServerContext* ctxt, const RegisterRequest *request, RegisterResponse *response) override {
        std::cout << "Hello from " << request->clientaddr() << std::endl;
        return Status::OK;
    }
};

int main() {

    MasterImpl service;
    ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    server->Wait();

    return 0;
}
