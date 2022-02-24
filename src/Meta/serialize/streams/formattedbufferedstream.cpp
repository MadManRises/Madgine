#include "../../metalib.h"

#include "formattedbufferedstream.h"

#include "syncstreamdata.h"
#include "buffered_streambuf.h"
#include "../syncmanager.h"

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

    void FormattedBufferedStream::beginMessage()
    {
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).beginMessage();
    }

    void FormattedBufferedStream::endMessage()
    {
        static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).endMessage();
    }

    bool FormattedBufferedStream::isMessageAvailable()
    {
        int avail = 0;

        std::ios_base::iostate _State = std::ios_base::goodbit;
        if (!*this) {
            _State |= std::ios_base::failbit;
        } else if ((avail = static_cast<buffered_streambuf &>(mFormatter->stream().buffer()).in_avail()) < 0) {
            _State |= std::ios_base::eofbit;
        }
        mFormatter->stream().stream().setstate(_State);

        return avail > 0;
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