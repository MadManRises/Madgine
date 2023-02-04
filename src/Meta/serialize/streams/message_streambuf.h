#pragma once

#include "pendingrequest.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT message_streambuf : std::basic_streambuf<char> {

        message_streambuf() = default;
        message_streambuf(const message_streambuf &) = delete;

        message_streambuf &operator=(const message_streambuf &) = delete;

        void beginMessageWrite(ParticipantId requester = 0, MessageId requestId = 0, GenericMessageReceiver receiver = {});
        void endMessageWrite();

        MessageId beginMessageRead();
        std::streamsize endMessageRead();

        PendingRequest getRequest(MessageId id);

    protected:
        virtual MessageId beginMessageWriteImpl() = 0;
        virtual void endMessageWriteImpl() = 0;

        virtual MessageId beginMessageReadImpl() = 0;
        virtual std::streamsize endMessageReadImpl();

    private:
        std::list<PendingRequest> mPendingRequests;
    };

}
}
