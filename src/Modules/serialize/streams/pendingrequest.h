#pragma once

namespace Engine {
namespace Serialize {

    struct PendingRequest {
        TransactionId mId;
        ParticipantId mRequester;
        TransactionId mRequesterTransactionId;
        std::function<void(void *)> mCallback;
    };

}
}