#ifndef _WORKERIMPL_H
#define _WORKERIMPL_H


#include "worker.pb.h"
#include "worker.grpc.pb.h"
#include "master.grpc.pb.h"

#include <string>

class WorkerImpl final : public worker::WorkerService::Service {
    private:
        std::string mAddr_;   // Address of Master
        std::string hAddr_;
        std::unique_ptr<master::MasterService::Stub> stub_;

        void LoadFromXML(const std::string &);
    public:
        // Initializer
        WorkerImpl(const std::string &);

        inline const std::string GetServiceAddr() const {
            return hAddr_;
        }

        grpc::Status StartTask(grpc::ServerContext *,
                const worker::StartRequest *,
                worker::StartReply *) override;
        grpc::Status PushModel(grpc::ServerContext *,
                const worker::PushRequest *,
                worker::PushReply *) override;

};

#endif
