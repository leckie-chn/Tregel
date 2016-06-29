#ifndef _WORKERIMPL_H
#define _WORKERIMPL_H


#include "worker.pb.h"
#include "worker.grpc.pb.h"
#include "master.grpc.pb.h"

#include <string>
#include <map>
#include <cstdint>

class WorkerImpl final : public worker::WorkerService::Service {
    private:
        std::string mAddr_;   // Address of Master
        std::string hAddr_;
        std::unique_ptr<master::MasterService::Stub> mStub_;            // RPC Stub on Master
        std::map<std::string, std::unique_ptr<worker::WorkerService::Stub> > prStubs_;    // RPC Stub on Peer Workers

        uint64_t vertexb_;   // beginning of vertex range
        // uint64_t vertexe;   // end of vertex range

        inline void LoadFromXML(const std::string &);
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
