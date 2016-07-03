#ifndef _MASTERIMPL_H
#define _MASTERIMPL_H

#include "master.grpc.pb.h"
#include "master.pb.h"
#include "worker.grpc.pb.h"
#include "worker.pb.h"

#include <grpc++/grpc++.h>
#include <string>
#include <map>
#include <pthread.h>

#include "src/util/XmlLoader.h"


class MasterImpl final : public master::MasterService::Service {
    private:

        // Client on Worker Node
        class WorkerC {
            public:
                // Vars for Barrier Sync
                bool waiting_;
                bool converge_;
                WorkerC();
        };

        XmlLoader conf_;
        const std::string servAddr;

        // mutex for critical data
        pthread_mutex_t mu_;
        // using `unique_ptr<WorkerC>` instead of `WorkerC` because of `unique_ptr<Stub>`
        std::map<std::string, WorkerC> Workers_;


        // Round Sync Vars
        int roundno_;   // stands for the maximum round number that all workers have complete (except for failures)
        pthread_cond_t cond_;   // condition variable on release all blocking workers

        // determine if the compute has come to an end
        bool haltRound();
        bool halt_;
        

        

    public:
        MasterImpl(const std::string &);

        inline const std::string & GetServiceAddr() const {
            return servAddr;
        }

        grpc::Status Barrier(grpc::ServerContext *,
                const master::BarrierRequest *,
                master::BarrierReply *) override;
};

#endif
