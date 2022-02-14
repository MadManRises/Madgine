#include "../../metalib.h"

#include "syncstreamdata.h"

#include "../formatter.h"
#include "../syncmanager.h"

namespace Engine {
namespace Serialize {

    PendingRequest *SyncStreamData::fetchRequest(TransactionId id)
    {
        if (id == 0)
            return nullptr;
        assert(mPendingRequests.front().mId == id);
        return &mPendingRequests.front();
    }

    void SyncStreamData::popRequest(TransactionId id)
    {
        assert(id != 0);
        assert(mPendingRequests.front().mId == id);
        mPendingRequests.pop();
    }

    TransactionId SyncStreamData::createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback)
    {
        if (requesterTransactionId == 0 && !callback)
            return 0;

        assert(callback || requester != 0);
        TransactionId id = ++mRunningTransactionId;
        mPendingRequests.push({ id, requester, requesterTransactionId, std::move(callback) });
        return id;
    }

    SyncStreamData::SyncStreamData(SyncManager &mgr, ParticipantId id)
        : SerializeStreamData(mgr, id)
    {
    }

    SyncManager *SyncStreamData::manager()
    {
        return static_cast<SyncManager *>(SerializeStreamData::manager());
    }

}
}