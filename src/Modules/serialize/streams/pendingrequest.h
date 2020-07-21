#pragma once

namespace Engine {
namespace Serialize {

    struct PendingRequest {

        PendingRequest(TransactionId id, ParticipantId request, TransactionId requesterTransactionId, std::function<void(void *)> callback)
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
            , mCallback(std::exchange(other.mCallback, std::function<void(void *)> {}))
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
                mCallback = std::function<void(void *)> {};
            }
        }

        TransactionId mId;
        ParticipantId mRequester;
        TransactionId mRequesterTransactionId;

    private:
        std::function<void(void *)> mCallback;
    };

}
}