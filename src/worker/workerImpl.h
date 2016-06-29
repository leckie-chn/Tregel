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

        inline void LoadFromXML(const std::string &);
        bool flag[];
        
        std::map<int, float> nodes;
        std::map<int, int> edges;
        void LoadFromXML(const std::string &);
        void loadFromDisk(std::map<int, float> &, std::map<int, int> &);
        void writeToDisk(std::map<int, float> &);
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
        grpc::Status InformNewPeer(grpc::ServerContext *,
                const worker::InformRequest *,
                worker::InformReply *) override;
};

#endif
