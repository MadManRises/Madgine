#include "madginelib.h"
#include "serializableunit.h"

#include "Streams/bufferedstream.h"

#include "serializablebase.h"

#include "serializemanager.h"

#include "toplevelserializableunit.h"

#include "observable.h"

namespace Engine {
namespace Serialize {

	SerializableUnit::SerializableUnit(TopLevelSerializableUnit *topLevel) :
	mTopLevel(topLevel),
	mMasterId((InvPtr)0)
{
}

SerializableUnit::~SerializableUnit()
{

}

void SerializableUnit::writeState(SerializeOutStream & out) const
{
	out << static_cast<InvPtr>(reinterpret_cast<uintptr_t>(this));
	for (Serializable *val : mStateValues) {
		val->writeState(out);
	}
}

void SerializableUnit::readState(SerializeInStream & in)
{
	InvPtr id;
	in >> id;
	if (mMasterId != id) {
		assert(mMasterId == (InvPtr)0);
		in.manager().addMapping(id, this);
		mMasterId = id;
	}
	for (Serializable *val : mStateValues) {
		val->readState(in);
	}
	
}

void SerializableUnit::readAction(SerializeInStream & in)
{
	
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
		if (!isAction && !mTopLevel->isMaster())
			throw 0;
		result = mTopLevel->getMasterMessageTargets(this);
		for (BufferedOutStream *out : result) {
			writeHeader(*out, isAction);
		}
	}
	return result;
	
}

BufferedOutStream * SerializableUnit::getSlaveMessageTarget()
{
	BufferedOutStream* result = nullptr;
	if (mTopLevel) {
		result = mTopLevel->getSlaveMessageTarget();
		writeHeader(*result, true);
	}
	return result;
}

void SerializableUnit::writeHeader(SerializeOutStream & out, bool isAction)
{
	MessageHeader header;
	header.mType = isAction ? (out.manager().isMaster() ? ACTION : REQUEST) : STATE;
	if (type() == NO_TOP_LEVEL) {
		header.mObject = out.manager().isMaster() ? static_cast<InvPtr>(reinterpret_cast<uintptr_t>(this)) : mMasterId;
		header.mIsMadgineComponent = false;
	}
	else {
		header.mMadgineComponent = type();
		header.mIsMadgineComponent = true;
	}
	out.write(header);
}

TopLevelMadgineObject SerializableUnit::type() {
	return NO_TOP_LEVEL;
}


void SerializableUnit::writeCreationData(SerializeOutStream & out) const
{
}

TopLevelSerializableUnit * SerializableUnit::topLevel()
{
	return mTopLevel;
}

void SerializableUnit::clearMasterId()
{
	mMasterId = (InvPtr)0;
}

/*InvPtr SerializableUnit::masterId()
{
	return mMasterId;
}

void SerializableUnit::setMasterId(InvPtr id)
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

