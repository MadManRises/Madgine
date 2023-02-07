#include "../../metalib.h"
#include "message_streambuf.h"

namespace Engine {
namespace Serialize {

    void message_streambuf::beginMessageWrite(ParticipantId requester, MessageId requestId, GenericMessageReceiver receiver)
    {
        assert(!pptr());
        MessageId id = beginMessageWriteImpl();
        assert(pptr());
        if (requester || receiver)
            mPendingRequests.push_back({ id, requester, requestId, std::move(receiver) });
    }

    void message_streambuf::endMessageWrite()
    {
        assert(pptr());
        endMessageWriteImpl();
        setp(nullptr, nullptr);
    }

    MessageId message_streambuf::beginMessageRead()
    {
        assert(!egptr());
        MessageId id = beginMessageReadImpl();
        assert(!id || egptr());
        return id;
    }

    std::streamsize message_streambuf::endMessageRead()
    {
        std::streamsize remainder = endMessageReadImpl();
        setg(nullptr, nullptr, nullptr);
        return remainder;
    }

    PendingRequest message_streambuf::getRequest(MessageId id)
    {
        auto it = std::ranges::find(mPendingRequests, id, &PendingRequest::mId);
        if (it == mPendingRequests.end())
            return { id };
        else {
            PendingRequest result = std::move(*it);
            mPendingRequests.erase(it);
            return result;
        }
    }

    std::streamsize message_streambuf::endMessageReadImpl() {
        return showmanyc();
    }

}
}
