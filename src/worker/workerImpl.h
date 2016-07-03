#ifndef _WORKERIMPL_H
#define _WORKERIMPL_H


#include "worker.pb.h"
#include "worker.grpc.pb.h"
#include "master.grpc.pb.h"

#include <string>
#include <pthread.h>

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

        void LoadFromXML(const std::string &);
        void loadFromDisk(std::map<int, float> &, std::map<int, std::vector<int>> &);
        void writeToDisk(std::map<int, float> &);
        bool pull(WorkerC &);

        // void shutdown();

        // thread id for main thread computing model
        pthread_t pid_;
        friend void *compute_thread(void *);

    public:
        // Constuctor 
        WorkerImpl(const std::string &);

        inline const std::string GetServiceAddr() const {
            return hAddr_;
        }

        grpc::Status PullModel(grpc::ServerContext *,
                const worker::PullRequest *,
                worker::PullReply *) override;

        std::map<int, float> nodes;
        std::map<int, float> local_nodes;
        std::map<int, std::vector<int>> edges;
        std::map<int, int> out_degree;
};

void *compute_thread(void *);
void page_rank(WorkerImpl &);

#endif
