#ifndef _WORKERIMPL_H
#define _WORKERIMPL_H


#include "worker.pb.h"
#include "worker.grpc.pb.h"
#include "master.grpc.pb.h"

#include <string>

class WorkerImpl final : public worker::WorkerService::Service {
    private:
        class WorkerC {
            public:
                const std::string addr_;
                std::unique_ptr<worker::WorkerService::Stub> stub_;
                WorkerC(const std::string &);
                bool hasmodel = false;
        };

        std::map<std::string, std::unique_ptr<WorkerC>> Workers_;
        std::string mAddr_;   // Address of Master
        std::string hAddr_;
        std::unique_ptr<master::MasterService::Stub> stub_;
        int startid;
        int endid;
        int version;
        
        std::map<int, float> nodes;
        std::map<int, float> local_nodes;
        std::map<int, std::vector<int>> edges;
        std::map<int, int> out_degree;
        void LoadFromXML(const std::string &);
        void loadFromDisk(std::map<int, float> &, std::map<int, std::vector<int>> &);
        void writeToDisk(std::map<int, float> &);
        void page_rank();
        bool pull(WorkerC &);
    public:
        // Initializer
        WorkerImpl(const std::string &);

        inline const std::string GetServiceAddr() const {
            return hAddr_;
        }

        grpc::Status StartTask(grpc::ServerContext *,
                const worker::StartRequest *,
                worker::StartReply *) override;
        grpc::Status PullModel(grpc::ServerContext *,
                const worker::PullRequest *,
                worker::PullReply *) override;
        grpc::Status InformNewPeer(grpc::ServerContext *,
                const worker::InformRequest *,
                worker::InformReply *) override;
};

#endif
