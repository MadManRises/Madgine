#include "../../metalib.h"
#include "message_streambuf.h"

namespace Engine {
namespace Serialize {

    void message_streambuf::beginMessageWrite(ParticipantId requester, MessageId requestId, GenericMessagePromise promise)
    {
        assert(!pptr());
        MessageId id = beginMessageWriteImpl();
        assert(pptr());
        if (requester || promise)
            mPendingRequests.push_back({ id, requester, requestId, std::move(promise) });
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
        return beginMessageReadImpl();
    }

    std::streamsize message_streambuf::endMessageRead()
    {
        std::streamsize remainder = endMessageReadImpl();
        setg(nullptr, nullptr, nullptr);
        return remainder;
    }

    PendingRequest message_streambuf::getRequest(MessageId id)
    {
        auto it = std::ranges::find_if(mPendingRequests, [&](const PendingRequest &r) { return r.mId == id; });
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
