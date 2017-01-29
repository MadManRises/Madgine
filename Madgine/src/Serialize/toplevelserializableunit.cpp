#include "madginelib.h"
#include "toplevelserializableunit.h"

#include "Streams/serializestream.h"

#include "serializablebase.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {
	TopLevelSerializableUnit::TopLevelSerializableUnit(Serialize::TopLevelMadgineObject type) :
		mType(type),
		mSlaveManager(0)
	{
		setTopLevel(this);
	}

	TopLevelSerializableUnit::~TopLevelSerializableUnit()
	{
		if (mSlaveManager) {
			mSlaveManager->removeTopLevelItem(this);
		}
		while (!mMasterManagers.empty()) {
			mMasterManagers.front()->removeTopLevelItem(this);
		}
	}

	bool TopLevelSerializableUnit::isMaster()
	{
		return mSlaveManager == 0;
	}

	TopLevelMadgineObject TopLevelSerializableUnit::topLevelType()
	{
		return mType;
	}

	std::list<BufferedOutStream*> TopLevelSerializableUnit::getMasterMessageTargets(SerializableUnit * unit)
{
	std::list<BufferedOutStream*> result;
	for (SerializeManager *mgr : mMasterManagers) {
		result.splice(result.end(), mgr->getMasterMessageTargets(unit));
	}
	return result;
}

	BufferedOutStream * TopLevelSerializableUnit::getSlaveMessageTarget()
	{
		if (mSlaveManager) {
			return mSlaveManager->getSlaveMessageTarget();
		}
		return nullptr;
	}

	SerializeManager * TopLevelSerializableUnit::getSlaveManager()
	{
		return mSlaveManager;
	}

bool TopLevelSerializableUnit::addManager(SerializeManager * mgr)
{
	if (mgr->isMaster()) {
		mMasterManagers.push_back(mgr);
	}
	else {
		if (mSlaveManager)
			return false;
		mSlaveManager = mgr;
	}
	return true;
}

void TopLevelSerializableUnit::removeManager(SerializeManager * mgr)
{

	if (mgr->isMaster()) {
		mMasterManagers.remove(mgr);
	}
	else {
		assert(mSlaveManager == mgr);
		mSlaveManager = 0;

	}
}

bool TopLevelSerializableUnit::updateManagerType(SerializeManager *mgr, bool isMaster)
{
	if (isMaster) {
		removeManager(mgr);
		mMasterManagers.push_back(mgr);
	}
	else {
		if (mSlaveManager)
			return false;
		removeManager(mgr);
		mSlaveManager = mgr;
	}
	return true;
}


} // namespace Serialize
} // namespace Core

