#include "../../metalib.h"
#include "serializestream.h"
#include "../serializeexception.h"

#include "../serializemanager.h"

#include "../serializable.h"

#include "serializestreambuf.h"

//#include "../../keyvalue/valuetype.h"

#include "Generic/bytebuffer.h"

#include "../formatter.h"

#include "../serializableunit.h"

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
        format().setupStream(mStream);
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other)
        : InStream(std::move(other))
        , mData(std::exchange(other.mData, nullptr))
    {
        format().setupStream(mStream);
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other, SerializeManager *mgr)
        : InStream(std::move(other))
        , mData(std::exchange(other.mData, nullptr))
    {
        data().setManager(mgr);
        format().setupStream(mStream);
    }

    SerializeInStream::~SerializeInStream()
    {
        if (mOwning) {
            assert(mData);
            delete mData;
            mData = nullptr;
        }
    }

    void SerializeInStream::readUnformatted(SyncableUnitBase *&p)
    {
        UnitId ptr;
        readUnformatted(ptr);
        if (ptr)
            ptr = (ptr << 2) | static_cast<UnitId>(UnitIdTag::SYNCABLE);
        p = reinterpret_cast<SyncableUnitBase *>(ptr);
    }

    void SerializeInStream::readUnformatted(SerializableDataUnit *&p)
    {
        uint32_t ptr;
        readUnformatted(ptr);
        assert(ptr <= (std::numeric_limits<uint32_t>::max() >> 2));
        if (ptr)
            ptr = (ptr << 2) | static_cast<UnitId>(UnitIdTag::SERIALIZABLE);
        p = reinterpret_cast<SerializableDataUnit *>(ptr);
    }

    void SerializeInStream::setNextFormattedStringDelimiter(char c)
    {
        assert(!(c && mNextFormattedStringDelimiter));
        mNextFormattedStringDelimiter = c;
    }

    void SerializeInStream::readUnformatted(std::string &s)
    {
        if (format().mBinary) {
            uint32_t size;
            readRaw(size);
            s.resize(size);
            readRaw(&s[0], size);
        } else {
            if (mNextFormattedStringDelimiter) {
                if (!std::getline(mStream, s, mNextFormattedStringDelimiter))
                    std::terminate();
                mNextFormattedStringDelimiter = 0;
            } else {
                if (!InStream::operator>>(s))
                    std::terminate();
            }
        }
    }

    void SerializeInStream::readUnformatted(ByteBuffer &b)
    {
        assert(format().mBinary);
        uint32_t size;
        readRaw(size);
        std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(size);
        readRaw(buffer.get(), size);
        b = ByteBuffer { std::move(buffer), size };
    }

    void SerializeInStream::readUnformatted(std::monostate &)
    {
    }

    void SerializeInStream::readRaw(void *buffer, size_t size)
    {
        InStream::readRaw(buffer, size);
        if (!*this) {
            throw SerializeException(
                "Deserialization Failure");
        }
    }

    std::string SerializeInStream::readN(size_t n)
    {
        if (n == 0)
            return {};

        skipWs();

        assert(!format().mBinary);
        std::string buffer(n, '\0');
        readRaw(&buffer[0], n);
        return buffer;
    }

    std::string SerializeInStream::peekN(size_t n)
    {
        assert(!format().mBinary);

        pos_type pos = tell();
        std::string result = readN(n);
        seek(pos);
        return result;
    }

    std::string SerializeInStream::readUntil(const char *c)
    {
        assert(!format().mBinary);

        char firstNonWs = ' ';

        while (std::isspace(firstNonWs)) {

            if (InStream::readRaw(&firstNonWs, 1) == 0)
                return {};
        }

        char buffer[255];
        size_t i = 0;
        buffer[i++] = firstNonWs;
        do {
            if (InStream::readRaw(&buffer[i], 1) == 0)
                break;
            ++i;
        } while (!strchr(c, buffer[i - 1]));
        buffer[i] = '\0';

        return buffer;
    }

    std::string SerializeInStream::peekUntil(const char *c)
    {
        assert(!format().mBinary);

        pos_type pos = tell();
        std::string result = readUntil(c);
        seek(pos);
        return result;
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

    Formatter &SerializeInStream::format() const
    {
        return data().format();
    }

    bool SerializeInStream::isBinary() const
    {
        return format().mBinary;
    }

    SerializeStreamData &SerializeInStream::data() const
    {
        return *mData;
    }

    SerializableUnitList &SerializeInStream::serializableList()
    {
        return data().serializableList();
    }

    void SerializeInStream::startSerializableRead(SerializableListHolder *list)
    {
        data().startSerializableRead(list);
    }

    SerializeOutStream::SerializeOutStream()
    {
    }

    SerializeOutStream::SerializeOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SerializeStreamData> data)
        : OutStream(std::move(buffer))
        , mData(std::move(data))
    {
        format().setupStream(mStream);
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other)
        : OutStream(std::move(other))
        , mData(std::move(other.mData))
    {
        format().setupStream(mStream);
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr)
        : OutStream(std::move(other))
        , mData(std::move(other.mData))
    {
        data().setManager(mgr);
        format().setupStream(mStream);
    }

    SerializeOutStream::~SerializeOutStream() = default;

    ParticipantId SerializeOutStream::id() const
    {
        return data().id();
    }

    void SerializeOutStream::writeUnformatted(const SyncableUnitBase *p)
    {
        writeUnformatted(SerializeManager::convertPtr(manager(), *this, p));
    }

    void SerializeOutStream::writeUnformatted(const SerializableDataUnit *p)
    {
        uint32_t id = 0;
        if (p) {
            SerializableUnitMap &map = serializableMap();
            auto it = map.try_emplace(p, map.size() + 1).first;
            id = it->second;
        }
        writeUnformatted(id);
    }

    void SerializeOutStream::writeRaw(const void *buffer, size_t size)
    {
        OutStream::writeRaw(buffer, size);
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

    Formatter &SerializeOutStream::format() const
    {
        return data().format();
    }

    bool SerializeOutStream::isBinary() const
    {
        return format().mBinary;
    }

    void SerializeOutStream::writeUnformatted(const std::string &s)
    {
        writeUnformatted(std::string_view { s });
    }

    void SerializeOutStream::writeUnformatted(const std::string_view &s)
    {
        if (format().mBinary) {
            writeRaw<uint32_t>(s.size());
            writeRaw(s.data(), s.size());
        } else {
            OutStream::operator<<(s);
        }
    }

    void SerializeOutStream::writeUnformatted(const ByteBuffer &b)
    {
        assert(format().mBinary);
        writeRaw<uint32_t>(b.mSize);
        writeRaw(b.mData, b.mSize);
    }

    void SerializeOutStream::writeUnformatted(const std::monostate &)
    {
    }

    SerializableUnitMap &SerializeOutStream::serializableMap()
    {
        return data().serializableMap();
    }

    void SerializeOutStream::startSerializableWrite(SerializableMapHolder *map)
    {
        data().startSerializableWrite(map);
    }

}
} // namespace Scripting
