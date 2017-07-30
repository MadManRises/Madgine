#include "interfaceslib.h"
#include "serializestream.h"
#include "../serializeexception.h"




#include "../serializemanager.h"

#include "../serializable.h"

namespace Engine {
namespace Serialize {

SerializeInStream::SerializeInStream(std::istream &ifs, Serialize::SerializeManager &mgr, ParticipantId id) :
	Stream(mgr, id),
	mIfs(ifs)
{
}


SerializeInStream &SerializeInStream::operator >>(ValueType &result)
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
		read(&(temp[0]), size);
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
	case ValueType::Type::EndOfListValue:
		result = ValueType::EOL();
		break;
	case ValueType::Type::Vector3Value:
	{
		Vector3 a3;
		read(a3);
		result = a3;
		break;
	}
	case ValueType::Type::Vector4Value:
		std::array<float, 4> a4;
		read(a4);
		result = a4;
		break;
	case ValueType::Type::FloatValue:
		float f;
		read(f);
		result = f;
		break;
	case ValueType::Type::InvScopePtrValue:
	{
		InvScopePtr p;
		read(p);
		result = p;
		break;
	}
	default:
		throw Serialize::SerializeException(Exceptions::unknownDeserializationType);
	}
	mLog.logRead(result);
    return *this;
}


SerializeInStream & SerializeInStream::operator >> (Serializable & s)
{
	s.readState(*this);
	return *this;
}

void SerializeInStream::read(void *buffer, size_t size)
{
    mIfs.read((char*)buffer, size);
    if (!mIfs.good()) throw SerializeException(
                Exceptions::deserializationFailure);
}


pos_type SerializeInStream::tell()
{
    return mIfs.tellg();
}

void SerializeInStream::seek(pos_type p)
{
    mIfs.seekg(p);
}

bool SerializeInStream::loopRead() {
	pos_type pos = tell();
	ValueType::Type type;
	read(type);
	if (type == ValueType::Type::EndOfListValue)
		return false;
	seek(pos);
	return true;
}

SerializeInStream::operator bool()
{
    return (bool) mIfs;
}

SerializeOutStream::SerializeOutStream(std::ostream &ofs, Serialize::SerializeManager &mgr, ParticipantId id) :
	Stream(mgr, id),
    mOfs(ofs)
{

}

SerializeOutStream & SerializeOutStream::operator<<(const ValueType & v)
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
	case ValueType::Type::EndOfListValue:
		break;
	case ValueType::Type::ScopeValue:
		throw Serialize::SerializeException("Cannot Serialize a Scope-Pointer!");
		break;
	case ValueType::Type::Vector3Value:
		writeData(v.as<Vector3>().ptr(), sizeof(float) * 3);
		break;
	case ValueType::Type::Vector4Value:
		writeData(v.as<std::array<float, 4>>().data(), sizeof(float) * 4);
		break;
	case ValueType::Type::FloatValue:
		write(v.as<float>());
		break;
	case ValueType::Type::InvScopePtrValue:
		write(v.as<InvScopePtr>());
		break;
	default:
		throw Serialize::SerializeException(Exceptions::unknownSerializationType);
	}
	mLog.logWrite(v);
	return *this;
}

SerializeOutStream & SerializeOutStream::operator<<(SerializableUnitBase * p)
{
	write(ExtendedValueType::SerializableUnitValue);
	write(mManager.convertPtr(*this, p));
	return *this;
}

SerializeOutStream & SerializeOutStream::operator<<(const Serializable & s)
{
	s.writeState(*this);
	return *this;
}

void SerializeOutStream::writeData(const void *buffer, size_t size)
{
    mOfs.write((char *)buffer, size);
}

pos_type SerializeOutStream::tell()
{
    return mOfs.tellp();
}

void SerializeOutStream::seek(pos_type p)
{
	mOfs.seekp(p);
}

Stream::Stream(SerializeManager &mgr, ParticipantId id) :
	mManager(mgr),
	mId(id),
	mLog(this)
{
}

SerializeManager &Stream::manager()
{
	return mManager;
}

bool Stream::isMaster()
{
	return mManager.isMaster(this);
}

Util::Process & Stream::process()
{
	return mManager.process();
}

ParticipantId Stream::id()
{
	return mId;
}

void Stream::setId(ParticipantId id)
{
	mId = id;
}

SerializeOutStream::operator bool()
{
	return (bool)mOfs;
}

}
} // namespace Scripting

