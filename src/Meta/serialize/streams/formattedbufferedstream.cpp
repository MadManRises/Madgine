#include "../../metalib.h"

#include "formattedbufferedstream.h"

#include "../syncmanager.h"
#include "buffered_streambuf.h"
#include "syncstreamdata.h"

namespace Engine {
namespace Serialize {

    FormattedBufferedStream::FormattedBufferedStream(std::unique_ptr<Formatter> format, std::unique_ptr<message_streambuf> buffer, std::unique_ptr<SyncStreamData> data)
        : FormattedSerializeStream(std::move(format), { std::move(buffer), std::move(data) })
    {
    }

    FormattedBufferedStream::FormattedBufferedStream(FormattedBufferedStream &&other, SyncManager *mgr)
        : FormattedBufferedStream(std::move(other))
    {
        mFormatter->stream().data()->setManager(mgr);
    }

    void FormattedBufferedStream::beginMessageWrite()
    {
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).beginMessageWrite();
        mFormatter->beginMessageWrite();
    }

    void FormattedBufferedStream::beginMessageWrite(ParticipantId requester, MessageId requestId, GenericMessageReceiver receiver)
    {
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).beginMessageWrite(requester, requestId, std::move(receiver));
        mFormatter->beginMessageWrite();
    }

    void FormattedBufferedStream::endMessageWrite()
    {
        mFormatter->endMessageWrite();
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).endMessageWrite();
    }

    StreamResult FormattedBufferedStream::beginMessageRead(MessageReadMarker &msg)
    {
        if (mFormatter) {
            MessageId id = static_cast<message_streambuf &>(mFormatter->stream().buffer()).beginMessageRead();
            if (id != 0) {
                mFormatter->stream().clear();
                STREAM_PROPAGATE_ERROR(mFormatter->beginMessageRead());
                msg = { id, mFormatter.get() };
            }
        }
        return {};
    }

    FormattedBufferedStream::MessageReadMarker::~MessageReadMarker()
    {
    }

    FormattedBufferedStream::MessageReadMarker &FormattedBufferedStream::MessageReadMarker::operator=(MessageReadMarker &&other)
    {
        assert(!mId);
        mId = std::exchange(other.mId, 0);
        mFormatter = std::move(other.mFormatter);
        return *this;
    }

    StreamResult FormattedBufferedStream::MessageReadMarker::end()
    {        
        assert(mId);
        STREAM_PROPAGATE_ERROR(mFormatter->endMessageRead());
        mFormatter->stream().skipWs();
        mFormatter->stream().clear();
        if (static_cast<message_streambuf&>(mFormatter->stream().buffer()).endMessageRead() > 0) {
            printf("Message not fully read!");
        }
        mFormatter = nullptr;
        mId = 0;
        return {};
    }

    FormattedBufferedStream::MessageReadMarker::operator bool() const
    {
        return mId != 0;
    }

    PendingRequest FormattedBufferedStream::getRequest(MessageId id)
    {
        return static_cast<message_streambuf &>(mFormatter->stream().buffer()).getRequest(id);
    }

    FormattedBufferedStream &FormattedBufferedStream::sendMessages()
    {
        if (mFormatter)
            mFormatter->stream().stream().flush();
        return *this;
    }

    StreamResult FormattedBufferedStream::beginHeaderRead()
    {
        return mFormatter->beginHeaderRead();
    }

    StreamResult FormattedBufferedStream::endHeaderRead()
    {
        return mFormatter->endHeaderRead();
    }

    void FormattedBufferedStream::beginHeaderWrite()
    {
        mFormatter->beginHeaderWrite();
    }

    void FormattedBufferedStream::endHeaderWrite()
    {
        mFormatter->endHeaderWrite();
    }

}
}