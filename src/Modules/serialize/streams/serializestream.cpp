#include "../../moduleslib.h"
#include "serializestream.h"
#include "../serializeexception.h"

#include "../serializemanager.h"

#include "../serializable.h"

#include "../../keyvalue/valuetype.h"

namespace Engine {
namespace Serialize {

    SerializeInStream::SerializeInStream(std::unique_ptr<SerializeStreambuf> &&buffer)
        : InStream(std::move(buffer))
        , mLog(*this)
    {
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other)
        : InStream(std::move(other))
        , mLog(*this)
    {
    }

    SerializeInStream::SerializeInStream(SerializeInStream &&other, SerializeManager *mgr)
        : InStream(std::move(other))
        , mLog((buffer().setManager(mgr), *this))
    {
    }

    void SerializeInStream::read(ValueType &result)
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
        case ValueType::Type::InvScopePtrValue: {
            InvScopePtr p;
            readRaw(p);
            result = p;
            break;
        }
        default:
            throw SerializeException(Database::Exceptions::unknownDeserializationType);
        }
        //mLog.logRead(result);
    }

    void SerializeInStream::read(SerializableUnitBase *&p)
    {
        int type;
        readRaw(type);
        if (type != SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<SerializableUnitBase *>)
            throw SerializeException(Database::Exceptions::notValueType("SerializableUnit"));
        size_t ptr;
        readRaw(ptr);
        p = convertPtr(ptr);
    }

    void SerializeInStream::read(std::string &s)
    {
        int type;
        readRaw(type);
        if (type != SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<std::string>)
            throw SerializeException(Database::Exceptions::notValueType("std::string"));
        decltype(std::declval<std::string>().size()) size;
        readRaw(size);
        s.resize(size);
        readRaw(&s[0], size);
    }

    void SerializeInStream::readRaw(void *buffer, size_t size)
    {
        if (!InStream::readRaw(buffer, size))
            throw SerializeException(
                Database::Exceptions::deserializationFailure);
    }

    bool SerializeInStream::loopRead()
    {
        pos_type pos = tell();
        int type;
        readRaw(type);
        if (type == SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<EOLType>) {
            EOLType eol;
            readRaw(eol);
            mLog.log(eol);
            return false;
        }
        seek(pos);
        return true;
    }

	std::string SerializeInStream::readPlainN(size_t n)
    {
        std::string buffer(n, '\0');
        readRaw(&buffer[0], n);
        return buffer;
    }

	std::string SerializeInStream::readPlainUntil(char c)
    {
        char buffer[255];
        size_t i = 0;
        do {
            if (!InStream::readRaw(&buffer[i], 1))
                break;
			++i;
        } while (buffer[i-1] != c);
        buffer[i] = '\0';

        return buffer;
    }

	std::string SerializeInStream::peekPlainUntil(char c)
    {
        pos_type pos = tell();
        std::string result = readPlainUntil(c);
        seek(pos);
        return result;
    }

	bool SerializeInStream::loopReadPlain(Formatter &format)
    {
            /*pos_type pos = tell();
            int type;
            readRaw(type);
            if (type == SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<EOLType>) {
                EOLType eol;
                readRaw(eol);
                mLog.log(eol);
                return false;
            }
            seek(pos);
        return true;*/
        throw 0;
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
        return buffer().isMaster();
    }

    SerializeInStream::SerializeInStream(SerializeStreambuf *buffer)
        : InStream(buffer)
        , mLog(*this)
    {
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
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other)
        : OutStream(std::move(other))
        , mLog(*this)
    {
    }

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other, SerializeManager *mgr)
        : OutStream(std::move(other))
        , mLog((buffer().setManager(mgr), *this))
    {        
    }

    ParticipantId SerializeOutStream::id() const
    {
        return buffer().id();
    }

    SerializeOutStream &SerializeOutStream::operator<<(const ValueType &v)
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
        case ValueType::Type::InvScopePtrValue:
            writeRaw(v.as<InvScopePtr>());
            break;
        default:
            throw SerializeException(Database::Exceptions::unknownSerializationType);
        }
        //mLog.logWrite(v);
        return *this;
    }

    SerializeOutStream &SerializeOutStream::operator<<(SerializableUnitBase *p)
    {
        writeRaw<int>(SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<SerializableUnitBase *>);
        writeRaw(manager()->convertPtr(*this, p));
        return *this;
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
        return buffer().isMaster();
    }

    /*pos_type SerializeOutStream::tell() const
		{
			return mOfs.tellp();
		}

		void SerializeOutStream::seek(pos_type p)
		{
			mOfs.seekp(p);
		}*/

    SerializeStreambuf &SerializeOutStream::buffer() const
    {
        return static_cast<SerializeStreambuf &>(OutStream::buffer());
    }

    SerializeOutStream &SerializeOutStream::operator<<(const std::string &s)
    {
        writeRaw<int>(SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<std::string>);
        writeRaw(s.size());
        writeRaw(s.c_str(), s.size());
        return *this;
    }

    SerializeStreambuf::SerializeStreambuf(SerializeManager &mgr, ParticipantId id)
        : mManager(&mgr)
        , mId(id)
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

    bool SerializeStreambuf::isMaster()
    {
        return !mManager || mManager->isMaster(this);
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
