#ifndef _WORKERIMPL_H
#define _WORKERIMPL_H


#include "../rpc/worker.pb.h"
#include "../rpc/worker.grpc.pb.h"
#include "../rpc/master.grpc.pb.h"

#include <string>

class WorkerImpl final : public worker::WorkerService::Service {
    private:
        const std::string maddr_;   // Address of Master
        const std::string addrme_;
        const std::unique_ptr<master::MasterService::Stub> stub_;
    public:
        // Initializer
        WorkerImpl(const std::string &, const std::string &);

        grpc::Status StartTask(grpc::ServerContext *,
                const worker::StartRequest *,
                worker::StartReply *) override;
        grpc::Status PushModel(grpc::ServerContext *,
                const worker::PushRequest *,
                worker::PushReply *) override;

};

#endif
