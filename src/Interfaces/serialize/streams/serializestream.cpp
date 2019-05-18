#include "../../interfaceslib.h"
#include "serializestream.h"
#include "../serializeexception.h"

#include "../serializemanager.h"

#include "../serializable.h"

#include "../../generic/valuetype.h"

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

    SerializeInStream::SerializeInStream(SerializeInStream &&other, SerializeManager &mgr)
        : InStream(std::move(other))
        , mLog((buffer().setManager(mgr), *this))
    {
    }

    SerializeInStream &SerializeInStream::operator>>(ValueType &result)
    {
        ValueType::Type type;
        read(type);

        switch (type) {
        case ValueType::Type::BoolValue:
            bool b;
            read(b);
            result = b;
            break;
        case ValueType::Type::StringValue: {
            decltype(std::declval<std::string>().size()) size;
            read(size);
            std::string temp(size, ' ');
            read(&temp[0], size);
            result = temp;
            break;
        }
        case ValueType::Type::IntValue:
            int i;
            read(i);
            result = i;
            break;
        case ValueType::Type::UIntValue:
            size_t s;
            read(s);
            result = s;
            break;
        case ValueType::Type::NullValue:
            result.clear();
            break;
        case ValueType::Type::Vector2Value: {
            Vector2 a2;
            read(a2);
            result = a2;
            break;
        }
        case ValueType::Type::Vector3Value: {
            Vector3 a3;
            read(a3);
            result = a3;
            break;
        }
        case ValueType::Type::Vector4Value: {
            Vector4 a4;
            read(a4);
            result = a4;
            break;
        }
        case ValueType::Type::FloatValue:
            float f;
            read(f);
            result = f;
            break;
        case ValueType::Type::InvScopePtrValue: {
            InvScopePtr p;
            read(p);
            result = p;
            break;
        }
        default:
            throw SerializeException(Database::Exceptions::unknownDeserializationType);
        }
        //mLog.logRead(result);
        return *this;
    }

    SerializeInStream &SerializeInStream::operator>>(Serializable &s)
    {
        s.readState(*this);
        return *this;
    }

    SerializeInStream &SerializeInStream::operator>>(SerializableUnitBase *&p)
    {
        int type;
        read(type);
        if (type != SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<SerializableUnitBase *>)
            throw SerializeException(Database::Exceptions::notValueType("SerializableUnit"));
        size_t ptr;
        read(ptr);
        p = convertPtr(ptr);
        return *this;
    }

    SerializeInStream &SerializeInStream::operator>>(std::string &s)
    {
        int type;
        read(type);
        if (type != SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<std::string>)
            throw SerializeException(Database::Exceptions::notValueType("std::string"));
        decltype(std::declval<std::string>().size()) size;
        read(size);
        s.resize(size);
        read(&s[0], size);
        return *this;
    }

    void SerializeInStream::readRaw(void *buffer, size_t size)
    {
        read(buffer, size);
    }

    void SerializeInStream::read(void *buffer, size_t size)
    {
        if (!InStream::read(buffer, size))
            throw SerializeException(
                Database::Exceptions::deserializationFailure);
    }

    bool SerializeInStream::loopRead()
    {
        pos_type pos = tell();
        int type;
        read(type);
        if (type == SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<EOLType>) {
            EOLType eol;
            read(eol);
            mLog.log(eol);
            return false;
        }
        seek(pos);
        return true;
    }

    void SerializeInStream::logReadHeader(const MessageHeader &header, const std::string &object)
    {
        mLog.logBeginMessage(header, object);
    }

    SerializeManager &SerializeInStream::manager() const
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
        return manager().convertPtr(*this, ptr);
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

    SerializeOutStream::SerializeOutStream(SerializeOutStream &&other, SerializeManager &mgr)
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
        write(v.type());
        switch (v.type()) {
        case ValueType::Type::BoolValue:
            write(v.as<bool>());
            break;
        case ValueType::Type::StringValue: {
            const std::string &s = v.as<std::string>();
            auto size = s.size();
            write(size);
            writeData(s.c_str(), size);
            break;
        }
        case ValueType::Type::IntValue:
            write(v.as<int>());
            break;
        case ValueType::Type::UIntValue:
            write(v.as<size_t>());
            break;
        case ValueType::Type::NullValue:
            break;
        case ValueType::Type::ScopeValue:
            throw SerializeException("Cannot Serialize a Scope-Pointer!");
        case ValueType::Type::Vector2Value:
            writeData(v.as<Vector2>().ptr(), sizeof(float) * 2);
            break;
        case ValueType::Type::Vector3Value:
            writeData(v.as<Vector3>().ptr(), sizeof(float) * 3);
            break;
        case ValueType::Type::Vector4Value:
            writeData(v.as<Vector4>().ptr(), sizeof(float) * 4);
            break;
        case ValueType::Type::FloatValue:
            write(v.as<float>());
            break;
        case ValueType::Type::InvScopePtrValue:
            write(v.as<InvScopePtr>());
            break;
        default:
            throw SerializeException(Database::Exceptions::unknownSerializationType);
        }
        //mLog.logWrite(v);
        return *this;
    }

    SerializeOutStream &SerializeOutStream::operator<<(SerializableUnitBase *p)
    {
        write<int>(SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<SerializableUnitBase *>);
        write(manager().convertPtr(*this, p));
        return *this;
    }

    SerializeOutStream &SerializeOutStream::operator<<(const Serializable &s)
    {
        s.writeState(*this);
        return *this;
    }

    void SerializeOutStream::writeRaw(const void *buffer, size_t size)
    {
        writeData(buffer, size);
    }

    SerializeManager &SerializeOutStream::manager() const
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

    void SerializeOutStream::writeData(const void *data, size_t count)
    {
        OutStream::write(data, count);
    }

    SerializeStreambuf &SerializeOutStream::buffer() const
    {
        return static_cast<SerializeStreambuf &>(OutStream::buffer());
    }

    SerializeOutStream &SerializeOutStream::operator<<(const std::string &s)
    {
        write<int>(SERIALIZE_MAGIC_NUMBER + PrimitiveTypeIndex_v<std::string>);
        write(s.size());
        writeData(s.c_str(), s.size());
        return *this;
    }

    SerializeStreambuf::SerializeStreambuf(SerializeManager &mgr, ParticipantId id)
        : mManager(&mgr)
        , mId(id)
    {
    }

    void SerializeStreambuf::setManager(SerializeManager &mgr)
    {
        mManager = &mgr;
    }

    SerializeManager &SerializeStreambuf::manager()
    {
        return *mManager;
    }

    bool SerializeStreambuf::isMaster()
    {
        return mManager->isMaster(this);
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
