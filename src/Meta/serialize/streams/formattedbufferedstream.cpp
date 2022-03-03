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
    }

    void FormattedBufferedStream::endMessageWrite()
    {
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).endMessageWrite();
    }

    FormattedBufferedStream::MessageReadMarker FormattedBufferedStream::beginMessageRead()
    {
        if (mFormatter && static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).beginMessageRead()) {
            mFormatter->stream().clear();
            return { mFormatter.get() };
        } else {
            return {};
        }
    }

    FormattedBufferedStream::MessageReadMarker::~MessageReadMarker()
    {
    }

    void FormattedBufferedStream::MessageReadMarker::end()
    {
        assert(mFormatter);
        mFormatter->stream().skipWs();
        mFormatter->stream().clear();
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).endMessageRead();
        mFormatter = nullptr;
    }

    FormattedBufferedStream::MessageReadMarker::operator bool() const
    {
        return mFormatter;
    }

    FormattedBufferedStream &FormattedBufferedStream::sendMessages()
    {
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