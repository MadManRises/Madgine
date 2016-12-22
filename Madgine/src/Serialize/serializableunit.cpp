#include "madginelib.h"
#include "serializableunit.h"

#include "Streams/bufferedstream.h"

#include "serializablebase.h"

#include "serializemanager.h"

#include "toplevelserializableunit.h"

#include "observable.h"

namespace Engine {
namespace Serialize {

	SerializableUnit::SerializableUnit() :
	mTopLevel(0),
	mMasterId((InvPtr)0)
{
}

SerializableUnit::~SerializableUnit()
{

}

void SerializableUnit::writeState(SerializeOutStream & out) const
{
	for (Serializable *val : mStateValues) {
		val->writeState(out);
	}
}

void SerializableUnit::writeId(SerializeOutStream & out) const
{
	out << static_cast<InvPtr>(reinterpret_cast<uintptr_t>(this));
}

void SerializableUnit::readId(SerializeInStream & in)
{
	InvPtr id;
	in >> id;
	if (mMasterId != id) {
		assert(mMasterId == (InvPtr)0);
		in.manager().addMapping(id, this);
		mMasterId = id;
	}
}

void SerializableUnit::readState(SerializeInStream & in)
{
	for (Serializable *val : mStateValues) {
		val->readState(in);
	}
	
}

void SerializableUnit::readAction(BufferedInOutStream & in)
{
	int index;
	in >> index;
	mObservedValues.at(index)->readAction(in);
}

void SerializableUnit::readRequest(BufferedInOutStream & in)
{
	int index;
	in >> index;
	mObservedValues.at(index)->readRequest(in);
}

int SerializableUnit::addObservable(Observable * val)
{
	mObservedValues.push_back(val);
	return mObservedValues.size()-1;
}

void SerializableUnit::addSerializableValue(Serializable * val)
{
	mStateValues.push_back(val);
}

std::list<BufferedOutStream*> SerializableUnit::getMasterMessageTargets(bool isAction)
{
	std::list<BufferedOutStream*> result;
	if (mTopLevel) {
		result = mTopLevel->getMasterMessageTargets(this);

		for (BufferedOutStream *out : result) {
			writeMasterMessageHeader(*out, isAction);
		}
	}
	return result;
	
}

void SerializableUnit::writeMasterMessageHeader(BufferedOutStream & out, bool isAction)
{
	MessageHeader header;
	header.mType = isAction ? ACTION : STATE;
	if (type() == NO_TOP_LEVEL) {
		header.mObject = out.manager().convertPtr(out, this);
		header.mIsMadgineComponent = false;
	}
	else {
		header.mMadgineComponent = type();
		header.mIsMadgineComponent = true;
	}
	out.write(header);
}

BufferedOutStream * SerializableUnit::getSlaveMessageTarget()
{
	BufferedOutStream* result = nullptr;
	if (mTopLevel) {
		result = mTopLevel->getSlaveMessageTarget();
		MessageHeader header;
		header.mType = REQUEST;
		if (type() == NO_TOP_LEVEL) {
			header.mObject = result->manager().convertPtr(*result, this);
			header.mIsMadgineComponent = false;
		}
		else {
			header.mMadgineComponent = type();
			header.mIsMadgineComponent = true;
		}
		result->write(header);
	}
	return result;
}


TopLevelMadgineObject SerializableUnit::type() {
	return NO_TOP_LEVEL;
}


void SerializableUnit::writeCreationData(SerializeOutStream & out) const
{
}

void SerializableUnit::setTopLevel(TopLevelSerializableUnit * topLevel)
{
	mTopLevel = topLevel;
}

TopLevelSerializableUnit * SerializableUnit::topLevel()
{
	return mTopLevel;
}

void SerializableUnit::clearMasterId()
{
	mMasterId = (InvPtr)0;
}

InvPtr SerializableUnit::masterId()
{
	return mMasterId;
}

/*void SerializableUnit::setMasterId(InvPtr id)
{
	mMasterId = id;
}*/

void SerializableUnit::applySerializableMap(const std::map<InvPtr, SerializableUnit*>& map)
{
	for (Serializable *ser : mStateValues) {
		ser->applySerializableMap(map);
	}
}

} // namespace Serialize
} // namespace Core

