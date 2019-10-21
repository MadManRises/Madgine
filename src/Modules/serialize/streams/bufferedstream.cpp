#include "../../moduleslib.h"

#include "buffered_streambuf.h"
#include "bufferedstream.h"

#include "../serializableunit.h"
#include "../syncmanager.h"

#include "../messageheader.h"

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

    BufferedInStream::BufferedInStream(std::unique_ptr<buffered_streambuf> &&buffer)
        : SerializeInStream(std::move(buffer))
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

    bool BufferedInStream::isMessageAvailable() const
    {
        if (!*this)
            return false;
        return buffer().isMessageAvailable();
    }

    BufferedInStream::BufferedInStream(buffered_streambuf *buffer)
        : SerializeInStream(buffer)
    {
    }

    buffered_streambuf &BufferedInStream::buffer() const
    {
        return static_cast<buffered_streambuf &>(SerializeInStream::buffer());
    }

    void BufferedInStream::readHeader(MessageHeader &header)
    {
        if (!isMessageAvailable())
            std::terminate();
        readRaw(header);
    }

    BufferedOutStream::BufferedOutStream(
        std::unique_ptr<buffered_streambuf> &&buffer)
        : SerializeOutStream(std::move(buffer))
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

    void BufferedOutStream::beginMessage(const SerializableUnitBase *unit,
        MessageType type)
    {
        MessageHeader header;
        header.mType = type;
        header.mObject = SerializeManager::convertPtr(manager(), *this, unit);
        mLog.logBeginMessage(header, typeid(*unit).name());
        buffer().beginMessage();
        writeRaw(header);
    }

    void BufferedOutStream::beginMessage(Command cmd)
    {
        MessageHeader header;
        header.mCmd = cmd;
        header.mObject = SERIALIZE_MANAGER;
        mLog.logBeginMessage(header, manager()->name());
        buffer().beginMessage();
        writeRaw(header);
    }

    void BufferedOutStream::endMessage() { buffer().endMessage(); }

    int BufferedOutStream::sendMessages() { 
		if (!*this)
            return -1;
		return buffer().sendMessages(); 
	}

    SyncManager *BufferedOutStream::manager() const
        {
            return static_cast<SyncManager*>(SerializeOutStream::manager());
        }

        buffered_streambuf &BufferedOutStream::buffer() const
    {
        return static_cast<buffered_streambuf &>(SerializeOutStream::buffer());
    }

    BufferedOutStream &BufferedOutStream::operator<<(BufferedInStream &in)
    {
        OutStream::operator<<(&in.buffer());
        return *this;
    }

    BufferedInOutStream::BufferedInOutStream(
        std::unique_ptr<buffered_streambuf> &&buffer)
        : BufferedInStream(buffer.get())
        , BufferedOutStream(std::move(buffer))
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

    StreamError BufferedInOutStream::error() const { return buffer().closeCause(); }

    bool BufferedInOutStream::isClosed() const
    {
        return !bool(*this) || buffer().isClosed();
    }

    void BufferedInOutStream::close()
    {
        writeCommand(STREAM_EOF);
        buffer().close();
    }

    BufferedInOutStream::operator bool() const
    {
        return this->SerializeInStream::operator bool() && this->SerializeOutStream::operator bool();
    }
} // namespace Serialize
} // namespace Engine
