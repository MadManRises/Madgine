#include "madginelib.h"
#include "toplevelserializableunit.h"

#include "Streams/serializestream.h"

#include "serializablebase.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {
	TopLevelSerializableUnit::TopLevelSerializableUnit(Serialize::TopLevelMadgineObject type) :
		SerializableUnit(this, type),
		mSlaveManager(0)
	{
	}

	TopLevelSerializableUnit::~TopLevelSerializableUnit()
	{
	}

	bool TopLevelSerializableUnit::isMaster()
	{
		return false;
	}

	std::list<SerializeOutStream*> TopLevelSerializableUnit::getMasterMessageTargets(SerializableUnit * unit)
{
	std::list<SerializeOutStream*> result;
	for (SerializeManager *mgr : mMasterManagers) {
		result.splice(result.end(), mgr->getMessageTargets(unit));
	}
	return result;
}

	SerializeOutStream * TopLevelSerializableUnit::getSlaveMessageTarget()
	{
		if (mSlaveManager) {
			return mSlaveManager->getStream();
		}
		return nullptr;
	}

void TopLevelSerializableUnit::addManager(SerializeManager * mgr)
{
	if (mgr->isMaster()) {
		mMasterManagers.push_back(mgr);
	}
	else {
		if (mSlaveManager)
			throw 0;
		mSlaveManager = mgr;
	}
}

void TopLevelSerializableUnit::removeManager(SerializeManager * mgr)
{
	if (mgr->isMaster()) {
		mMasterManagers.remove(mgr);
	}
	else {
		if (mSlaveManager != mgr)
			throw 0;
		mSlaveManager = 0;
	}
}


} // namespace Serialize
} // namespace Core

