#include "interfaceslib.h"
#include "serializestream.h"
#include "../serializeexception.h"


#include "exceptionmessages.h"

#include "../serializemanager.h"

namespace Engine {
namespace Serialize {

SerializeInStream::SerializeInStream(std::istream &ifs, Serialize::SerializeManager &mgr) :
    mIfs(ifs),
	Stream(mgr)
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
		std::array<float, 3> a;
		read(a);
		result = a;
		break;
	case ValueType::Type::FloatValue:
		float f;
		read(f);
		result = f;
		break;
	case ValueType::Type::InvPtrValue:
		InvPtr p;
		read(p);
		result = p;
		break;
	default:
		throw Serialize::SerializeException(Exceptions::unknownDeserializationType);
	}
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
	ValueType v;
	(*this) >> v;
	if (v.isEOL())
		return false;
	seek(pos);
	return true;
}

SerializeInStream::operator bool()
{
    return (bool) mIfs;
}

SerializeOutStream::SerializeOutStream(std::ostream &ofs, Serialize::SerializeManager &mgr) :
	Stream(mgr),
    mOfs(ofs)
{

}

SerializeOutStream & SerializeOutStream::operator<<(const ValueType & v)
{
	write(v.isScope() ? ValueType::Type::InvPtrValue : v.type());
	switch (v.type()) {
	case ValueType::Type::BoolValue:
		write(v.asBool());
		break;
	case ValueType::Type::StringValue: {
		const std::string &s = v.asString();
		auto size = s.size();
		write(size);
		writeData(s.c_str(), size);
		break;
	}
	case ValueType::Type::IntValue:
		write(v.asInt());
		break;
	case ValueType::Type::UIntValue:
		write(v.asUInt());
		break;
	case ValueType::Type::NullValue:
		break;
	case ValueType::Type::EndOfListValue:
		break;
	case ValueType::Type::ScopeValue:
		write(v.asScope());
		break;
	case ValueType::Type::Vector3Value:
		writeData(v.asVector3().data(), sizeof(float) * 3);
		break;
	case ValueType::Type::FloatValue:
		write(v.asFloat());
		break;
	case ValueType::Type::InvPtrValue:
		write(v.asInvPtr());
		break;
	default:
		throw Serialize::SerializeException(Exceptions::unknownSerializationType);
	}
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

Stream::Stream(SerializeManager &mgr) :
	mManager(mgr),
	mId(0)
{
}

SerializeManager &Stream::manager()
{
	return mManager;
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


}
} // namespace Scripting

