#ifndef _MASTERIMPL_H
#define _MASTERIMPL_H

#include "../rpc/master.grpc.pb.h"
#include "../rpc/master.pb.h"
#include "../rpc/worker.grpc.pb.h"

#include <grpc++/grpc++.h>
#include <string>
#include <vector>

#include <pthread.h>


class MasterImpl final : public master::MasterService::Service {
    private:
        const int workerN_;

        // Client on Worker Node
        class WorkerC {
            public:
                const std::string addr_;
                const std::unique_ptr<worker::WorkerService::Stub> stub_;
                WorkerC(const std::string &);
        };

        pthread_mutex_t mtxWorkers_;
        std::vector<std::unique_ptr<WorkerC>> Workers_;

        void StartJob();        

    public:
        MasterImpl(int);

        grpc::Status Register(grpc::ServerContext *,
                const master::RegisterRequest *,
                master::RegisterReply*) override;
        grpc::Status Barrier(grpc::ServerContext *,
                const master::BarrierRequest *,
                master::BarrierReply *) override;
};

#endif
