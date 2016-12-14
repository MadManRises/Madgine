#include "madginelib.h"
#include "serializestream.h"
#include "../serializeexception.h"


#include "Database/exceptionmessages.h"

#include "Os/os.h"

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
	result.readState(*this);
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

SerializeOutStream::SerializeOutStream(std::ostream &ofs, Serialize::SerializeManager &mgr) :
	Stream(mgr),
    mOfs(ofs)
{

}

SerializeOutStream & SerializeOutStream::operator<<(const ValueType & v)
{
	v.writeState(*this);
	return *this;
}

SerializeOutStream & SerializeOutStream::operator<<(const Ogre::Quaternion & q)
{
	return *this << *q.ptr() << Ogre::Vector3(q.ptr() + 1);
}

void SerializeOutStream::beginMessage()
{
}

void SerializeOutStream::endMessage()
{
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

UI::Process & Stream::process()
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

