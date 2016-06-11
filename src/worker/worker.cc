#include <iostream>
#include <string>
#include <grpc++/grpc++.h>

#include "../rpc/master.grpc.pb.h"

using grpc::Channel;
using grpc::CreateChannel;
using grpc::InsecureChannelCredentials;
using grpc::ClientContext;
using master::MasterService;
using master::RegisterRequest;
using master::RegisterResponse;


int main(int argc, char **argv) {
    auto channel = CreateChannel("127.0.0.1:50051", InsecureChannelCredentials()); 
    auto stub = MasterService::NewStub(channel);
    RegisterRequest request;
    RegisterResponse response;
    request.set_clientaddr(std::string(argv[1]));
    ClientContext ctxt;

    auto status = stub->Register(&ctxt, request, &response);
    if (!status.ok()) {
        std::cout << "rpc failed" << std::endl;
    }


    return 0;
}
