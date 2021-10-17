#pragma once

#include "Generic/lambda.h"

namespace Engine {
namespace Serialize {

    struct PendingRequest {

        PendingRequest(TransactionId id, ParticipantId request, TransactionId requesterTransactionId, Lambda<void(void *)> callback)
            : mId(id)
            , mRequester(request)
            , mRequesterTransactionId(requesterTransactionId)
            , mCallback(std::move(callback))
        {
        }

        PendingRequest(PendingRequest &&other)
            : mId(std::exchange(other.mId, 0))
            , mRequester(std::exchange(other.mRequester, 0))
            , mRequesterTransactionId(std::exchange(other.mRequesterTransactionId, 0))
            , mCallback(std::exchange(other.mCallback, Lambda<void(void *)> {}))
        {
        }

        ~PendingRequest()
        {
            assert(!mCallback);
        }

        void operator()(void *data)
        {
            if (mCallback) {
                mCallback(data);
                mCallback = Lambda<void(void *)> {};
            }
        }

        TransactionId mId;
        ParticipantId mRequester;
        TransactionId mRequesterTransactionId;

    private:
        Lambda<void(void *)> mCallback;
    };

}
}