#include "madginelib.h"
#include "serializableunit.h"

#include "Streams/serializestream.h"

#include "serializablebase.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {


SerializableUnit::SerializableUnit() :
	mMasterManager(0)
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

std::list<SerializeOutStream*> SerializableUnit::getMessageTargets(bool isAction)
{
	if (mMasterManager) {
		std::list<SerializeOutStream*> result = mMasterManager->getMessageTargets(this);
		for (SerializeOutStream *out : result) {
			writeHeader(*out, isAction);
		}
		return result;
	}
	return{};
	
}

void SerializableUnit::setMasterManager(SerializeManager * mgr)
{
	mMasterManager = mgr;
}

void SerializableUnit::writeHeader(SerializeOutStream & out, bool isAction)
{
	MessageHeader header;
	header.mType = isAction ? (out.manager().isMaster() ? ACTION : REQUEST) : STATE;
	TopLevelMadgineObject ob = topLevelId();
	if (ob == NO_TOPLEVEL_ITEM) {
		header.mObject = out.manager().isMaster() ? static_cast<InvPtr>(reinterpret_cast<uintptr_t>(this)) : masterId();
		header.mIsMadgineComponent = false;
	}
	else {
		header.mMadgineComponent = ob;
		header.mIsMadgineComponent = true;
	}

	out.write(header);
}

TopLevelMadgineObject SerializableUnit::topLevelId()
{
	return NO_TOPLEVEL_ITEM;
}



void SerializableUnit::writeCreationData(SerializeOutStream & out) const
{
}

} // namespace Serialize
} // namespace Core

