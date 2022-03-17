#include "../../metalib.h"

#include "formattedbufferedstream.h"

#include "../syncmanager.h"
#include "buffered_streambuf.h"
#include "syncstreamdata.h"

namespace Engine {
namespace Serialize {

    FormattedBufferedStream::FormattedBufferedStream(std::unique_ptr<Formatter> format, std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SyncStreamData> data)
        : FormattedSerializeStream(std::move(format), { std::make_unique<buffered_streambuf>(std::move(buffer)), std::move(data) })
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

    void FormattedBufferedStream::endMessageWrite()
    {
        mFormatter->endMessageWrite();
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).endMessageWrite();
    }

    StreamResult FormattedBufferedStream::beginMessageRead(MessageReadMarker &msg)
    {
        if (mFormatter && static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).beginMessageRead()) {
            mFormatter->stream().clear();
            STREAM_PROPAGATE_ERROR(mFormatter->beginMessageRead());
            msg = { mFormatter.get() };
            return {};
        } else {
            return {};
        }
    }

    FormattedBufferedStream::MessageReadMarker::~MessageReadMarker()
    {
    }

    FormattedBufferedStream::MessageReadMarker &FormattedBufferedStream::MessageReadMarker::operator=(MessageReadMarker &&other)
    {
        assert(!mFormatter);
        mFormatter = std::move(other.mFormatter);
        return *this;
    }

    StreamResult FormattedBufferedStream::MessageReadMarker::end()
    {
        assert(mFormatter);
        STREAM_PROPAGATE_ERROR(mFormatter->endMessageRead());
        mFormatter->stream().skipWs();
        mFormatter->stream().clear();
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).endMessageRead();
        mFormatter = nullptr;
        return {};
    }

    FormattedBufferedStream::MessageReadMarker::operator bool() const
    {
        return mFormatter;
    }

    FormattedBufferedStream &FormattedBufferedStream::sendMessages()
    {
        if (mFormatter)
            mFormatter->stream().stream().flush();
        return *this;
    }

    TransactionId FormattedBufferedStream::createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback)
    {
        return static_cast<SyncStreamData *>(mFormatter->stream().data())->createRequest(requester, requesterTransactionId, std::move(callback));
    }

    PendingRequest *FormattedBufferedStream::fetchRequest(TransactionId id)
    {
        return static_cast<SyncStreamData *>(mFormatter->stream().data())->fetchRequest(id);
    }

    void FormattedBufferedStream::popRequest(TransactionId id)
    {
        static_cast<SyncStreamData *>(mFormatter->stream().data())->popRequest(id);
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