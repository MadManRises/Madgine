#include "madginelib.h"
#include "serializableunit.h"

#include "Streams/serializestream.h"

#include "serializablebase.h"

#include "serializemanager.h"

#include "toplevelserializableunit.h"

namespace Engine {
namespace Serialize {


SerializableUnit::SerializableUnit(TopLevelSerializableUnit *topLevel, TopLevelMadgineObject type) :
	mTopLevel(topLevel),
	mObject(type)
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

void SerializableUnit::readState(SerializeInStream & in)
{
	for (Serializable *val : mStateValues) {
		val->readState(in);
	}
	
}

void SerializableUnit::readAction(SerializeInStream & in)
{
	
}

void SerializableUnit::readRequest(SerializeInStream & in)
{
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

std::list<SerializeOutStream*> SerializableUnit::getMasterMessageTargets(bool isAction)
{
	std::list<SerializeOutStream*> result;
	if (mTopLevel) {
		result = mTopLevel->getMasterMessageTargets(this);
		for (SerializeOutStream *out : result) {
			writeHeader(*out, isAction);
		}
	}
	return result;
	
}

void SerializableUnit::writeHeader(SerializeOutStream & out, bool isAction)
{
	MessageHeader header;
	header.mType = isAction ? (out.manager().isMaster() ? ACTION : REQUEST) : STATE;
	if (mObject == NO_TOP_LEVEL) {
		header.mObject = out.manager().isMaster() ? static_cast<InvPtr>(reinterpret_cast<uintptr_t>(this)) : masterId();
		header.mIsMadgineComponent = false;
	}
	else {
		header.mMadgineComponent = mObject;
		header.mIsMadgineComponent = true;
	}
	out.write(header);
}


SerializeOutStream * SerializableUnit::getSlaveMessageTarget()
{
	SerializeOutStream* result = nullptr;
	if (mTopLevel) {
		result = mTopLevel->getSlaveMessageTarget();
		if (result) {
			writeHeader(*result, true);
		}
	}
	return result;
}

void SerializableUnit::writeCreationData(SerializeOutStream & out) const
{
}

TopLevelSerializableUnit * SerializableUnit::topLevel()
{
	return mTopLevel;
}

InvPtr SerializableUnit::masterId()
{
	return mMasterId;
}

void SerializableUnit::setMasterId(InvPtr id)
{
	mMasterId = id;
}

void SerializableUnit::applySerializableMap(const std::map<InvPtr, SerializableUnit*>& map)
{
	for (Serializable *ser : mStateValues) {
		ser->applySerializableMap(map);
	}
}

} // namespace Serialize
} // namespace Core

