#include "../../moduleslib.h"
#include "serializestream.h"
#include "../serializeexception.h"

#include "../serializemanager.h"

#include "../serializable.h"

#include "serializestreambuf.h"

//#include "../../keyvalue/valuetype.h"

#include "Interfaces/filesystem/path.h"

#include "../../generic/bytebuffer.h"

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    SerializeInStream::SerializeInStream()
    {
    }

    SerializeInStream::SerializeInStream(std::unique_ptr<SerializeStreambuf> &&buffer)
        : InStream(std::move(buffer))
    {
        format().setupStream(mStream);
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other)
        : InStream(std::move(other))
    {
        format().setupStream(mStream);
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other, SerializeManager *mgr)
        : InStream(std::move(other))        
    {
        buffer().setManager(mgr);
        format().setupStream(mStream);
    }

    void SerializeInStream::readUnformatted(SerializableUnitBase *&p)
    {
        UnitId ptr;
        readUnformatted(ptr);
        if (ptr)
            ptr = (ptr << 1) | 0x1;
        p = reinterpret_cast<SerializableUnitBase *>(ptr);
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

    void SerializeInStream::readUnformatted(Filesystem::Path &p)
    {
        std::string s;
        readUnformatted(s);
        p = s;
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
        if (std::ios::iostate state = InStream::readRaw(buffer, size); state) {
            bool good = state & std::ios::goodbit;
            bool eof = state & std::ios::eofbit;
            bool fail = state & std::ios::failbit;
            bool bad = state & std::ios::badbit;
            throw SerializeException(
                Database::Exceptions::deserializationFailure);
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

        while (std::isspace(firstNonWs))
            if (InStream::readRaw(&firstNonWs, 1))
                return {};

        char buffer[255];
        size_t i = 0;
        buffer[i++] = firstNonWs;
        do {
            if (InStream::readRaw(&buffer[i], 1))
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
        return buffer().manager();
    }

    void SerializeInStream::setId(ParticipantId id)
    {
        buffer().setId(id);
    }

    ParticipantId SerializeInStream::id() const
    {
        return buffer().id();
    }

    bool SerializeInStream::isMaster()
    {
        return buffer().isMaster(StreamMode::READ);
    }

    SerializeInStream::SerializeInStream(SerializeStreambuf *buffer)
        : InStream(buffer)
    {
    }

    Formatter &SerializeInStream::format() const
    {
        return buffer().format();
    }

    bool SerializeInStream::isBinary() const
    {
        return format().mBinary;
    }

    SerializableUnitBase *SerializeInStream::convertPtr(UnitId ptr)
    {
        return manager()->convertPtr(*this, ptr);
    }

    SerializeStreambuf &SerializeInStream::buffer() const
    {
        return static_cast<SerializeStreambuf &>(InStream::buffer());
    }

    SerializeOutStream::SerializeOutStream()        
    {
    }

    SerializeOutStream::SerializeOutStream(std::unique_ptr<SerializeStreambuf> &&buffer)
        : OutStream(std::move(buffer))
    {
        format().setupStream(mStream);
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other)
        : OutStream(std::move(other))        
    {
        format().setupStream(mStream);
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr)
        : OutStream(std::move(other))        
    {
        buffer().setManager(mgr);
        format().setupStream(mStream);
    }

    ParticipantId SerializeOutStream::id() const
    {
        return buffer().id();
    }

    void SerializeOutStream::writeUnformatted(SerializableUnitBase *p)
    {
        writeUnformatted(SerializeManager::convertPtr(manager(), *this, p));
    }

    void SerializeOutStream::writeRaw(const void *buffer, size_t size)
    {
        OutStream::writeRaw(buffer, size);
    }

    SerializeManager *SerializeOutStream::manager() const
    {
        return buffer().manager();
    }

    bool SerializeOutStream::isMaster()
    {
        return buffer().isMaster(StreamMode::WRITE);
    }

    SerializeStreambuf &SerializeOutStream::buffer() const
    {
        return static_cast<SerializeStreambuf &>(OutStream::buffer());
    }

    Formatter &SerializeOutStream::format() const
    {
        return buffer().format();
    }

    bool SerializeOutStream::isBinary() const
    {
        return format().mBinary;
    }

    void SerializeOutStream::writeUnformatted(const std::string &s)
    {
        if (format().mBinary) {
            writeRaw<uint32_t>(s.size());
            writeRaw(s.c_str(), s.size());
        } else {
            OutStream::operator<<(s);
        }
    }

    void SerializeOutStream::writeUnformatted(const Filesystem::Path &p)
    {
        writeUnformatted(p.str());
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

}
} // namespace Scripting
