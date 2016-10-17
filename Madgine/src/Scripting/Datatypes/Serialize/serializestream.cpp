#include "libinclude.h"
#include "serializestream.h"
#include "serializeexception.h"


#include "Database/exceptionmessages.h"

#include "Os/os.h"

namespace Engine {
namespace Scripting {
namespace Serialize {

SerializeInStream::SerializeInStream(std::istream &ifs) :
    mIfs(ifs),
	mProcess(0)
{
}

SerializeInStream::SerializeInStream(std::istream & ifs, UI::Process & process) :
	mIfs(ifs),
	mProcess(&process)
{
}

SerializeInStream &SerializeInStream::operator >>(ValueType &result)
{

    result.clear();
    read(result.mType);

    switch (result.mType) {
    case ValueType::Type::BoolValue:
        read(result.mUnion.mBool);
        break;
    case ValueType::Type::StringValue: {
        decltype(std::declval<std::string>().size()) size;
        read(size);
		std::string *temp = new std::string(size, ' ');
        read(&(*temp)[0], size);
        result.mUnion.mString = temp;
        break;
    }
	case ValueType::Type::IntValue:
        read(result.mUnion.mInt);
        break;
	case ValueType::Type::UIntValue:
		read(result.mUnion.mUInt);
		break;
    case ValueType::Type::NullValue:
        break;
    case ValueType::Type::Vector3Value:
        float buffer[3];
        read(buffer);
        result.mUnion.mVector3 = new Ogre::Vector3(buffer);
        break;
    case ValueType::Type::FloatValue:
        read(result.mUnion.mFloat);
        break;
    case ValueType::Type::InvScopeValue:
        read(result.mUnion.mInvScope);
        break;
    default:
        throw SerializeException(Database::Exceptions::unknownDeserializationType);
    }
    return *this;
}

SerializeInStream & SerializeInStream::operator>>(Ogre::Quaternion & q)
{
	Ogre::Vector3 v;
	Ogre::Real w;
	*this >> w >> v;

	q = { w, v.x, v.y, v.z };
	
	return *this;
}


void SerializeInStream::read(void *buffer, size_t size)
{
    mIfs.read((char*)buffer, size);
    if (!mIfs.good()) throw SerializeException(
                Database::Exceptions::deserializationFailure);
}


pos_type SerializeInStream::tell()
{
    return mIfs.tellg();
}

void SerializeInStream::seek(pos_type p)
{
    mIfs.seekg(p);
}

SerializeInStream::operator bool()
{
    return (bool) mIfs;
}

UI::Process * SerializeInStream::process()
{
	return mProcess;
}

SerializeOutStream::SerializeOutStream(std::ostream &ofs) :
    mOfs(ofs)
{

}


SerializeOutStream &SerializeOutStream::operator <<(const ValueType &v)
{
    write(v.mType == ValueType::Type::ScopeValue ? ValueType::Type::InvScopeValue : v.mType);
    switch (v.mType) {
    case ValueType::Type::BoolValue:
        write(v.mUnion.mBool);
        break;
    case ValueType::Type::StringValue: {
        auto size = v.mUnion.mString->size();
        write(size);
        write(v.mUnion.mString->c_str(), size);
        break;
    }
    case ValueType::Type::IntValue:
        write(v.mUnion.mInt);
        break;
	case ValueType::Type::UIntValue:
		write(v.mUnion.mUInt);
		break;
    case ValueType::Type::NullValue:
        break;
    case ValueType::Type::ScopeValue:
        write(v.mUnion.mScope);
        break;
    case ValueType::Type::Vector3Value:
        write(v.mUnion.mVector3->ptr(), sizeof(float) * 3);
        break;
    case ValueType::Type::FloatValue:
        write(v.mUnion.mFloat);
        break;
    default:
        throw SerializeException(Database::Exceptions::unknownSerializationType);
    }
    return *this;
}

SerializeOutStream & SerializeOutStream::operator<<(const Ogre::Quaternion & q)
{
	return *this << *q.ptr() << Ogre::Vector3(q.ptr() + 1);
}

void SerializeOutStream::write(const void *buffer, size_t size)
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


}
}
} // namespace Scripting

