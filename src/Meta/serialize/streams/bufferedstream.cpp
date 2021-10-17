#include "../../metalib.h"

#include "buffered_streambuf.h"
#include "bufferedstream.h"

#include "../serializableunit.h"
#include "../syncmanager.h"

#include "../messageheader.h"

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    bool CompareStreamId::operator()(ParticipantId first,
        const BufferedInOutStream &second) const
    {
        return first < second.id();
    }

    bool CompareStreamId::operator()(const BufferedInOutStream &first,
        ParticipantId second) const
    {
        return first.id() < second;
    }

    bool CompareStreamId::operator()(BufferedOutStream *first,
        BufferedOutStream *second) const
    {
        return first->id() < second->id();
    }

    bool CompareStreamId::operator()(const BufferedInOutStream &first,
        const BufferedOutStream &second) const
    {
        return first.id() < second.id();
    }

    BufferedInStream::BufferedInStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<BufferedStreamData> data)
        : BufferedInStream(std::make_unique<buffered_streambuf>(std::move(buffer)), std::move(data))
    {
    }

    BufferedInStream::BufferedInStream(BufferedInStream &&other)
        : SerializeInStream(std::move(other))
    {
    }

    BufferedInStream::BufferedInStream(BufferedInStream &&other, SerializeManager *mgr)
        : SerializeInStream(std::move(other), mgr)
    {
    }

    BufferedInStream::BufferedInStream(std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<BufferedStreamData> data)
        : SerializeInStream(std::move(buffer), std::move(data))
    {
    }

    bool BufferedInStream::isMessageAvailable()
    {
        int avail = 0;

        std::ios_base::iostate _State = std::ios_base::goodbit;
        if (!*this) {
            _State |= std::ios_base::failbit;
        } else if ((avail = buffer().in_avail()) < 0) {
            _State |= std::ios_base::eofbit;
        }
        mStream.setstate(_State);

        return avail > 0;
    }

    PendingRequest *BufferedInStream::fetchRequest(TransactionId id)
    {
        return data().fetchRequest(id);
    }

    void BufferedInStream::popRequest(TransactionId id)
    {
        data().popRequest(id);
    }

    BufferedInStream::BufferedInStream(buffered_streambuf *buffer, BufferedStreamData *data)
        : SerializeInStream(buffer, data)
    {
    }

    BufferedStreamData &BufferedInStream::data() const
    {
        return static_cast<BufferedStreamData &>(SerializeInStream::data());
    }

    void BufferedInStream::readHeader(MessageHeader &header)
    {
        if (!isMessageAvailable())
            std::terminate();
        readRaw(header);
    }

    BufferedOutStream::BufferedOutStream(
        std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<BufferedStreamData> data)
        : BufferedOutStream(std::make_unique<buffered_streambuf>(std::move(buffer)), std::move(data))
    {
    }

    BufferedOutStream::BufferedOutStream(BufferedOutStream &&other)
        : SerializeOutStream(std::move(other))
    {
    }

    BufferedOutStream::BufferedOutStream(BufferedOutStream &&other, SerializeManager *mgr)
        : SerializeOutStream(std::move(other), mgr)
    {
    }

    BufferedOutStream::BufferedOutStream(
        std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<BufferedStreamData> data)
        : SerializeOutStream(std::move(buffer), std::move(data))
    {
    }

    buffered_streambuf& BufferedOutStream::buffer() const {
        return static_cast<buffered_streambuf &>(SerializeOutStream::buffer());
    }

    buffered_streambuf &BufferedInStream::buffer() const
    {
        return static_cast<buffered_streambuf &>(SerializeInStream::buffer());
    }

    void BufferedOutStream::beginMessage(const SyncableUnitBase *unit,
        MessageType type, TransactionId id)
    {
        MessageHeader header;
        header.mType = type;
        header.mObject = SerializeManager::convertPtr(manager(), *this, unit);
        header.mTransaction = id;
        buffer().beginMessage();
        writeRaw(header);
    }

    void BufferedOutStream::beginMessage(Command cmd)
    {
        MessageHeader header;
        header.mCmd = cmd;
        header.mObject = SERIALIZE_MANAGER;
        buffer().beginMessage();
        writeRaw(header);
    }

    void BufferedOutStream::endMessage()
    {
        buffer().endMessage();
    }

    BufferedOutStream &BufferedOutStream::sendMessages()
    {
        mStream.flush();
        return *this;
    }

    SyncManager *BufferedOutStream::manager() const
    {
        return static_cast<SyncManager *>(SerializeOutStream::manager());
    }

    TransactionId BufferedOutStream::createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback)
    {
        return data().createRequest(requester, requesterTransactionId, std::move(callback));
    }

    BufferedStreamData &BufferedOutStream::data() const
    {
        return static_cast<BufferedStreamData &>(SerializeOutStream::data());
    }

    BufferedInOutStream::BufferedInOutStream(
        std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<BufferedStreamData> data)
        : BufferedInOutStream(std::make_unique<buffered_streambuf>(std::move(buffer)), std::move(data))
    {
    }

    BufferedInOutStream::BufferedInOutStream(
        std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<BufferedStreamData> data)
        : BufferedInStream(buffer.get(), data.get())
        , BufferedOutStream(std::move(buffer), std::move(data))
    {
    }

    BufferedInOutStream::BufferedInOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<Formatter> format, SyncManager &mgr, ParticipantId id)
        : BufferedInOutStream(std::make_unique<buffered_streambuf>(std::move(buffer)), std::make_unique<BufferedStreamData>(std::move(format), mgr, id))
    {
    }

    BufferedInOutStream::BufferedInOutStream(BufferedInOutStream &&other)
        : BufferedInStream(std::move(other))
        , BufferedOutStream(std::move(other))
    {
    }

    BufferedInOutStream::BufferedInOutStream(BufferedInOutStream &&other,
        SerializeManager *mgr)
        : BufferedInStream(std::move(other), mgr)
        , BufferedOutStream(std::move(other), mgr)
    {
    }

    BufferedInOutStream::operator bool() const
    {
        return this->SerializeInStream::operator bool() && this->SerializeOutStream::operator bool();
    }
} // namespace Serialize
} // namespace Engine
