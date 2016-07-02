#ifndef _MASTERIMPL_H
#define _MASTERIMPL_H

#include "master.grpc.pb.h"
#include "master.pb.h"
#include "worker.grpc.pb.h"
#include "worker.pb.h"

#include <grpc++/grpc++.h>
#include <string>
#include <map>
#include <condition_variable>
#include <mutex>

#include "src/util/XmlLoader.h"


class MasterImpl final : public master::MasterService::Service {
    private:

        // Client on Worker Node
        class WorkerC {
            public:
                const std::string addr_;
                std::unique_ptr<worker::WorkerService::Stub> stub_;

                // Vars for Async StartTask
                std::unique_ptr<grpc::ClientAsyncResponseReader<worker::StartReply> > taskrpc_;
                worker::StartReply taskreply_;
                grpc::Status taskstat_;
                grpc::ClientContext taskcontext_;

                // Vars for Barrier Sync
                bool roundbarrier_;

                // Constructor
                WorkerC(const std::string &);
        };

        XmlLoader conf_;
        const std::string servAddr_;

        // using `unique_ptr<WorkerC>` instead of `WorkerC` because of `unique_ptr<Stub>`
        std::map<std::string, std::unique_ptr<WorkerC>> Workers_;

        // Private Methods
        // Start/Stop Tasks
        grpc::CompletionQueue jobcq_;
        void startJob();
        void stopJob();

        // Round Sync Vars
        int roundno_;
        std::mutex syncmtx_;
        std::unique_lock<std::mutex> synclk_;
        std::condition_variable synccond_;
        bool halt_;

        // failure handling
        void bcReboot(const std::string &);

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
