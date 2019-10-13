#include "../../moduleslib.h"
#include "serializestream.h"
#include "../serializeexception.h"

#include "../serializemanager.h"

#include "../serializable.h"

#include "../../keyvalue/valuetype.h"

#include "Interfaces//filesystem/path.h"

namespace Engine {
namespace Serialize {

    SerializeInStream::SerializeInStream(std::unique_ptr<SerializeStreambuf> &&buffer)
        : InStream(std::move(buffer))
        , mLog(*this)
    {
        format().setupStream(mStream);
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other)
        : InStream(std::move(other))
        , mLog(*this)
    {
        format().setupStream(mStream);
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other, SerializeManager *mgr)
        : InStream(std::move(other))
        , mLog((buffer().setManager(mgr), *this))
    {
        format().setupStream(mStream);
    }

    void SerializeInStream::read(ValueType &result, const char *name)
    {
        ValueType::Type type;
        readRaw(type);

        switch (type) {
        case ValueType::Type::BoolValue:
            bool b;
            readRaw(b);
            result = b;
            break;
        case ValueType::Type::StringValue: {
            decltype(std::declval<std::string>().size()) size;
            readRaw(size);
            std::string temp(size, ' ');
            readRaw(&temp[0], size);
            result = temp;
            break;
        }
        case ValueType::Type::IntValue:
            int i;
            readRaw(i);
            result = i;
            break;
        case ValueType::Type::UIntValue:
            size_t s;
            readRaw(s);
            result = s;
            break;
        case ValueType::Type::NullValue:
            result.clear();
            break;
        case ValueType::Type::Vector2Value: {
            Vector2 a2;
            readRaw(a2);
            result = a2;
            break;
        }
        case ValueType::Type::Vector3Value: {
            Vector3 a3;
            readRaw(a3);
            result = a3;
            break;
        }
        case ValueType::Type::Vector4Value: {
            Vector4 a4;
            readRaw(a4);
            result = a4;
            break;
        }
        case ValueType::Type::FloatValue:
            float f;
            readRaw(f);
            result = f;
            break;
        default:
            throw SerializeException(Database::Exceptions::unknownDeserializationType);
        }
        //mLog.logRead(result);
    }

    void SerializeInStream::readUnformatted(SerializableUnitBase *&p)
    {
        size_t ptr;
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
            decltype(std::declval<std::string>().size()) size;
            readRaw(size);
            s.resize(size);
            readRaw(&s[0], size);
        } else {
            if (mNextFormattedStringDelimiter) {
                if (!std::getline(mStream, s, mNextFormattedStringDelimiter))
                    throw 0;
                mNextFormattedStringDelimiter = 0;
            } else {
                if (!InStream::operator>>(s))
                    throw 0;
            }
        }
    }

    void SerializeInStream::readUnformatted(Filesystem::Path &p)
    {
        std::string s;
        readUnformatted(s);
        p = s;
    }

    void SerializeInStream::readRaw(void *buffer, size_t size)
    {
        if (!InStream::readRaw(buffer, size))
            throw SerializeException(
                Database::Exceptions::deserializationFailure);
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
            if (!InStream::readRaw(&firstNonWs, 1))
                return {};

        char buffer[255];
        size_t i = 0;
        buffer[i++] = firstNonWs;
        do {
            if (!InStream::readRaw(&buffer[i], 1))
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

    void SerializeInStream::logReadHeader(const MessageHeader &header, const std::string &object)
    {
        mLog.logBeginMessage(header, object);
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
        , mLog(*this)
    {
    }

    Formatter &SerializeInStream::format() const
    {
        return buffer().format();
    }

    SerializableUnitBase *SerializeInStream::convertPtr(size_t ptr)
    {
        return manager()->convertPtr(*this, ptr);
    }

    SerializeStreambuf &SerializeInStream::buffer() const
    {
        return static_cast<SerializeStreambuf &>(InStream::buffer());
    }

    SerializeOutStream::SerializeOutStream(std::unique_ptr<SerializeStreambuf> &&buffer)
        : OutStream(std::move(buffer))
        , mLog(*this)
    {
        format().setupStream(mStream);
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other)
        : OutStream(std::move(other))
        , mLog(*this)
    {
        format().setupStream(mStream);
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr)
        : OutStream(std::move(other))
        , mLog((buffer().setManager(mgr), *this))
    {
        format().setupStream(mStream);
    }

    ParticipantId SerializeOutStream::id() const
    {
        return buffer().id();
    }

    /*SerializeOutStream &SerializeOutStream::operator<<(const ValueType &v)
    {
        writeRaw(v.type());
        switch (v.type()) {
        case ValueType::Type::BoolValue:
            writeRaw(v.as<bool>());
            break;
        case ValueType::Type::StringValue: {
            const std::string &s = v.as<std::string>();
            auto size = s.size();
            writeRaw(size);
            writeRaw(s.c_str(), size);
            break;
        }
        case ValueType::Type::IntValue:
            writeRaw(v.as<int>());
            break;
        case ValueType::Type::UIntValue:
            writeRaw(v.as<size_t>());
            break;
        case ValueType::Type::NullValue:
            break;
        case ValueType::Type::ScopeValue:
            throw SerializeException("Cannot Serialize a Scope-Pointer!");
        case ValueType::Type::Vector2Value:
            writeRaw(v.as<Vector2>().ptr(), sizeof(float) * 2);
            break;
        case ValueType::Type::Vector3Value:
            writeRaw(v.as<Vector3>().ptr(), sizeof(float) * 3);
            break;
        case ValueType::Type::Vector4Value:
            writeRaw(v.as<Vector4>().ptr(), sizeof(float) * 4);
            break;
        case ValueType::Type::FloatValue:
            writeRaw(v.as<float>());
            break;
        default:
            throw SerializeException(Database::Exceptions::unknownSerializationType);
        }
        //mLog.logWrite(v);
        return *this;
    }*/

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

    void SerializeOutStream::writeUnformatted(const std::string &s)
    {
        if (format().mBinary) {
            writeRaw(s.size());
            writeRaw(s.c_str(), s.size());
        } else {
            OutStream::operator<<(s);
        }
    }

    void SerializeOutStream::writeUnformatted(const Filesystem::Path &p)
    {
        writeUnformatted(p.str());
    }

    SerializeStreambuf::SerializeStreambuf(std::unique_ptr<Formatter> format)
        : mFormatter(std::move(format))
    {
    }

    SerializeStreambuf::SerializeStreambuf(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id)
        : mManager(&mgr)
        , mId(id)
        , mFormatter(std::move(format))
    {
    }

    void SerializeStreambuf::setManager(SerializeManager *mgr)
    {
        mManager = mgr;
    }

    SerializeManager *SerializeStreambuf::manager()
    {
        return mManager;
    }

    Formatter &SerializeStreambuf::format() const
    {
        return *mFormatter;
    }

    bool SerializeStreambuf::isMaster(StreamMode mode)
    {
        if (mManager)
            return mManager->isMaster(this);
        return mode == StreamMode::WRITE;
    }

    ParticipantId SerializeStreambuf::id() const
    {
        return mId;
    }

    void SerializeStreambuf::setId(ParticipantId id)
    {
        mId = id;
    }
}
} // namespace Scripting
