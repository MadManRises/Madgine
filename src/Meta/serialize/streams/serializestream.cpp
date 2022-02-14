#include "../../metalib.h"
#include "serializestream.h"

#include "serializestreamdata.h"

//#include "../../keyvalue/valuetype.h"

#include "../formatter.h"

#include "Generic/cowstring.h"

#include "Generic/bytebuffer.h"

#include "base64/base64.h"

#include "../serializemanager.h"

namespace Engine {
namespace Serialize {

    SerializeInStream::SerializeInStream()
    {
    }

    SerializeInStream::SerializeInStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SerializeStreamData> data)
        : InStream(std::move(buffer))
        , mData(data.release())
    {
        assert(mData);
        //format().setupStream(mStream);
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other)
        : InStream(std::move(other))
        , mData(std::exchange(other.mData, nullptr))
    {
        //format().setupStream(mStream);
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other, SerializeManager *mgr)
        : InStream(std::move(other))
        , mData(std::exchange(other.mData, nullptr))
    {
        data().setManager(mgr);
        //format().setupStream(mStream);
    }

    SerializeInStream::~SerializeInStream()
    {
        if (mOwning) {
            assert(mData);
            delete mData;
            mData = nullptr;
        }
    }

    StreamResult SerializeInStream::read(void *buffer, size_t size)
    {
        InStream::read(buffer, size);
        if (!*this)
            return STREAM_PARSE_ERROR(*this, true, "Unexpected EOF");
        return {};
    }

    StreamResult SerializeInStream::readN(std::string &buffer, size_t n)
    {
        //assert(!format().mBinary);

        skipWs();

        if (n == 0)
            return {};

        buffer.resize(n, '\0');
        return read(&buffer[0], n);
    }

    StreamResult SerializeInStream::readUntil(std::string &buffer, const char *delim)
    {
        //assert(!format().mBinary);

        skipWs();

        buffer.resize(255);
        size_t i = 0;
        do {
            STREAM_PROPAGATE_ERROR(read(&buffer[i], 1));
            ++i;
        } while (!strchr(delim, buffer[i - 1]));
        buffer.resize(i);

        return {};
    }

    StreamResult SerializeInStream::peekN(std::string &buffer, size_t n)
    {
        //assert(!format().mBinary);

        pos_type pos = tell();
        STREAM_PROPAGATE_ERROR(readN(buffer, n));
        seek(pos);
        return {};
    }

    StreamResult SerializeInStream::peekUntil(std::string &buffer, const char *c)
    {
        //assert(!format().mBinary);

        pos_type pos = tell();
        STREAM_PROPAGATE_ERROR(readUntil(buffer, c));
        seek(pos);
        return {};
    }

    SerializeManager *SerializeInStream::manager() const
    {
        return data().manager();
    }

    void SerializeInStream::setId(ParticipantId id)
    {
        data().setId(id);
    }

    ParticipantId SerializeInStream::id() const
    {
        return data().id();
    }

    bool SerializeInStream::isMaster()
    {
        return data().isMaster(StreamMode::READ);
    }

    SerializeInStream::SerializeInStream(std::basic_streambuf<char> *buffer, SerializeStreamData *data)
        : InStream(buffer)
        , mData(data)
    {
        assert(mData);
    }

    SerializeStreamData &SerializeInStream::data() const
    {
        return *mData;
    }

    SerializableUnitList &SerializeInStream::serializableList()
    {
        return data().serializableList();
    }

    StreamResult SerializeInStream::read(SyncableUnitBase *&p)
    {
        UnitId ptr;
        STREAM_PROPAGATE_ERROR(read(ptr));
        assert(ptr <= (std::numeric_limits<UnitId>::max() >> 2));
        if (ptr)
            ptr = (ptr << 2) | static_cast<UnitId>(UnitIdTag::SYNCABLE);
        p = reinterpret_cast<SyncableUnitBase *>(ptr);
        return {};
    }

    StreamResult SerializeInStream::read(SerializableDataUnit *&p)
    {
        uint32_t ptr;
        STREAM_PROPAGATE_ERROR(read(ptr));
        assert(ptr <= (std::numeric_limits<uint32_t>::max() >> 2));
        if (ptr)
            ptr = (ptr << 2) | static_cast<uint32_t>(UnitIdTag::SERIALIZABLE);
        p = reinterpret_cast<SerializableDataUnit *>(ptr);
        return {};
    }

    StreamResult SerializeInStream::operator>>(SyncableUnitBase *&p)
    {
        UnitId ptr;
        STREAM_PROPAGATE_ERROR(operator>>(ptr));
        assert(ptr <= (std::numeric_limits<UnitId>::max() >> 2));
        if (ptr)
            ptr = (ptr << 2) | static_cast<UnitId>(UnitIdTag::SYNCABLE);
        p = reinterpret_cast<SyncableUnitBase *>(ptr);
        return {};
    }

    StreamResult SerializeInStream::operator>>(SerializableDataUnit *&p)
    {
        uint32_t ptr;
        STREAM_PROPAGATE_ERROR(operator>>(ptr));
        assert(ptr <= (std::numeric_limits<uint32_t>::max() >> 2));
        if (ptr)
            ptr = (ptr << 2) | static_cast<uint32_t>(UnitIdTag::SERIALIZABLE);
        p = reinterpret_cast<SerializableDataUnit *>(ptr);
        return {};
    }

    StreamResult SerializeInStream::read(std::string &s)
    {
        uint32_t size;
        STREAM_PROPAGATE_ERROR(read(size));
        s.resize(size);
        STREAM_PROPAGATE_ERROR(read(&s[0], size));
        return {};
    }

    StreamResult SerializeInStream::operator>>(std::string &s)
    {
        InStream::operator>>(s);
        if (!*this)
            return STREAM_PARSE_ERROR(*this, false, "Expected <string>");
        return {};
    }

    StreamResult SerializeInStream::read(ByteBuffer &b)
    {
        uint32_t size;
        STREAM_PROPAGATE_ERROR(read(size));
        std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(size);
        STREAM_PROPAGATE_ERROR(read(buffer.get(), size));
        b = ByteBuffer { std::move(buffer), size };
        return {};
    }

    StreamResult SerializeInStream::operator>>(ByteBuffer &b)
    {
        std::string base64Encoded;
        STREAM_PROPAGATE_ERROR(operator>>(base64Encoded));
        if (!Base64::decode(b, base64Encoded))
            return STREAM_PARSE_ERROR(*this, false, "Invalid Base64 String '" << base64Encoded << "'");
        return {};
    }

    StreamResult SerializeInStream::read(std::monostate &)
    {
        return {};
    }

    StreamResult SerializeInStream::operator>>(std::monostate &)
    {
        return {};
    }

    SerializeOutStream::SerializeOutStream()
    {
    }

    SerializeOutStream::SerializeOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SerializeStreamData> data)
        : OutStream(std::move(buffer))
        , mData(std::move(data))
    {
        //format().setupStream(mStream);
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other)
        : OutStream(std::move(other))
        , mData(std::move(other.mData))
    {
        //format().setupStream(mStream);
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr)
        : OutStream(std::move(other))
        , mData(std::move(other.mData))
    {
        data().setManager(mgr);
        //format().setupStream(mStream);
    }

    SerializeOutStream::~SerializeOutStream() = default;

    ParticipantId SerializeOutStream::id() const
    {
        return data().id();
    }

    SerializeManager *SerializeOutStream::manager() const
    {
        return data().manager();
    }

    bool SerializeOutStream::isMaster()
    {
        return data().isMaster(StreamMode::WRITE);
    }

    SerializeStreamData &SerializeOutStream::data() const
    {
        return *mData;
    }

    SerializableUnitMap &SerializeOutStream::serializableMap()
    {
        return data().serializableMap();
    }

    void SerializeOutStream::write(const SyncableUnitBase *p)
    {
        OutStream::write(SerializeManager::convertPtr(*this, p));
    }

    void SerializeOutStream::write(const SerializableDataUnit *p)
    {
        uint32_t id = 0;
        if (p) {
            SerializableUnitMap &map = serializableMap();
            auto it = map.try_emplace(p, map.size() + 1).first;
            id = it->second;
        }
        OutStream::write(id);
    }

    SerializeOutStream &SerializeOutStream::operator<<(const SyncableUnitBase *p)
    {
        OutStream::operator<<(SerializeManager::convertPtr(*this, p));
        return *this;
    }
    SerializeOutStream &SerializeOutStream::operator<<(const SerializableDataUnit *p)
    {
        uint32_t id = 0;
        if (p) {
            SerializableUnitMap &map = serializableMap();
            auto it = map.try_emplace(p, map.size() + 1).first;
            id = it->second;
        }
        OutStream::operator<<(id);
    }

    void SerializeOutStream::write(const std::string_view &s)
    {
        OutStream::write<uint32_t>(s.size());
        OutStream::write(s.data(), s.size());
    }

    SerializeOutStream &SerializeOutStream::operator<<(const std::string_view &s)
    {
        OutStream::operator<<(s);
        return *this;
    }

    void SerializeOutStream::write(const ByteBuffer &b)
    {
        OutStream::write<uint32_t>(b.mSize);
        OutStream::write(b.mData, b.mSize);
    }

    SerializeOutStream &SerializeOutStream::operator<<(const ByteBuffer &b)
    {
        OutStream::operator<<(Base64::encode(b));
        return *this;
    }

    void SerializeOutStream::write(const std::monostate &)
    {
    }

    SerializeOutStream &SerializeOutStream::operator<<(const std::monostate &)
    {
        return *this;
    }

}
} // namespace Scripting
