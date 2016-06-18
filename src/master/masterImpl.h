#ifndef _MASTERIMPL_H
#define _MASTERIMPL_H

#include "master.grpc.pb.h"
#include "master.pb.h"
#include "worker.grpc.pb.h"

#include <grpc++/grpc++.h>
#include <string>
#include <map>

#include <pthread.h>


class MasterImpl final : public master::MasterService::Service {
    private:

        // Client on Worker Node
        class WorkerC {
            public:
                const std::string addr_;
                const std::unique_ptr<worker::WorkerService::Stub> stub_;
                WorkerC(const std::string &);
        };

        // mutex for Workers_
        pthread_mutex_t mtxWorkers_;
        // using `unique_ptr<WorkerC>` instead of `WorkerC` because of `unique_ptr<Stub>`
        std::map<std::string, std::unique_ptr<WorkerC>> Workers_;

        unsigned unRegWorkerN_;
        std::string servAddr_;

        void StartJobs();        
        void LoadFromXML(const std::string &);

    public:
        MasterImpl(const std::string &);

        inline const std::string & GetServiceAddr() const {
            return servAddr_;
        }

        grpc::Status Register(grpc::ServerContext *,
                const master::RegisterRequest *,
                master::RegisterReply*) override;
        grpc::Status Barrier(grpc::ServerContext *,
                const master::BarrierRequest *,
                master::BarrierReply *) override;
};

#endif
