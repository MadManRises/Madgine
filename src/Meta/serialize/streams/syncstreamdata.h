#pragma once

#include "serializestreamdata.h"

#include "pendingrequest.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT SyncStreamData : SerializeStreamData {
    public:
        SyncStreamData(SyncManager &mgr, ParticipantId id = 0);
        SyncStreamData(const SyncStreamData &) = delete;
        SyncStreamData(SyncStreamData &&) noexcept = delete;

        virtual ~SyncStreamData() = default;

        SyncManager *manager();

        //read
        PendingRequest *fetchRequest(TransactionId id);
        void popRequest(TransactionId id);

        //write
        TransactionId createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback);

    private:
        TransactionId mRunningTransactionId = 0;
        std::queue<PendingRequest> mPendingRequests;
    };

}
}