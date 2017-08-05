#include "interfaceslib.h"
#include "toplevelserializableunit.h"

#include "streams/serializestream.h"


#include "serializemanager.h"

namespace Engine {
namespace Serialize {

	TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(size_t staticId) :
		SerializableUnitBase(this, staticId),
		mSlaveManager(0),
		mStaticSlaveId(staticId)
	{
	}

	TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase & other) :
		SerializableUnitBase(this, other),
		mSlaveManager(0),
		mStaticSlaveId(other.mStaticSlaveId)
	{		
	}

	TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(TopLevelSerializableUnitBase && other) :
		SerializableUnitBase(this, std::forward<TopLevelSerializableUnitBase>(other)),
		mSlaveManager(0),
		mStaticSlaveId(other.mStaticSlaveId)
	{		
		while (!other.mMasterManagers.empty()) {
			other.mMasterManagers.front()->moveTopLevelItem(&other, this);
		}
		if (other.mSlaveManager) {
			other.mSlaveManager->moveTopLevelItem(&other, this);
		}
	}

	

	TopLevelSerializableUnitBase::~TopLevelSerializableUnitBase()
	{
		if (mSlaveManager) {
			mSlaveManager->removeTopLevelItem(this);
		}
		while (!mMasterManagers.empty()) {
			mMasterManagers.front()->removeTopLevelItem(this);
		}
	}

	void TopLevelSerializableUnitBase::copyStaticSlaveId(const TopLevelSerializableUnitBase & other)
	{
		mStaticSlaveId = other.mStaticSlaveId;
	}

	bool TopLevelSerializableUnitBase::isMaster()
	{
		return mSlaveManager == 0;
	}

	ParticipantId TopLevelSerializableUnitBase::getLocalMasterParticipantId()
	{
		return SerializeManager::getLocalMasterParticipantId();
	}

	ParticipantId TopLevelSerializableUnitBase::getSlaveParticipantId()
	{
		return mSlaveManager->getSlaveParticipantId();
	}

	void TopLevelSerializableUnitBase::setStaticSlaveId(size_t staticId)
	{
		mStaticSlaveId = staticId;
	}

	void TopLevelSerializableUnitBase::initSlaveId()
	{
		return setSlaveId(mStaticSlaveId ? mStaticSlaveId : mMasterId.first);
	}

	size_t TopLevelSerializableUnitBase::readId(SerializeInStream & in)
	{
		mStaticSlaveId = SerializableUnitBase::readId(in);		
		return mStaticSlaveId;
	}

	std::list<BufferedOutStream*> TopLevelSerializableUnitBase::getMasterMessageTargets(SerializableUnitBase * unit, MessageType type, const std::list<ParticipantId> &targets)
{
	std::list<BufferedOutStream*> result;
	if (targets.empty()) {
		for (SerializeManager *mgr : mMasterManagers) {
			result.splice(result.end(), mgr->getMasterMessageTargets(unit, type));
		}
	}
	else {
		std::list<ParticipantId> filterCopy = targets;
		auto f = [&](SerializableUnitBase *unit, ParticipantId id) {
			auto it = std::find(filterCopy.begin(), filterCopy.end(), id);
			if (it != filterCopy.end()) {
				filterCopy.erase(it);
				return true;
			}
			return false;
		};
		for (SerializeManager *mgr : mMasterManagers) {
			result.splice(result.end(), mgr->getMasterMessageTargets(unit, type, f));
		}
		if (!filterCopy.empty())
			LOG_WARNING("Passed invalid Particpant-Id as Message Target!");
	}
	return result;
}

	BufferedOutStream * TopLevelSerializableUnitBase::getSlaveMessageTarget(SerializableUnitBase *unit)
	{
		if (mSlaveManager) {
			return mSlaveManager->getSlaveMessageTarget(unit);
		}
		return nullptr;
	}

	const std::list<SerializeManager*>& TopLevelSerializableUnitBase::getMasterManagers()
	{
		return mMasterManagers;
	}

	SerializeManager * TopLevelSerializableUnitBase::getSlaveManager()
	{
		return mSlaveManager;
	}

bool TopLevelSerializableUnitBase::addManager(SerializeManager * mgr)
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

void TopLevelSerializableUnitBase::removeManager(SerializeManager * mgr)
{

	if (mgr->isMaster()) {
		size_t check = mMasterManagers.size();
		mMasterManagers.remove(mgr);
		assert(mMasterManagers.size() == check - 1);
	}
	else {
		assert(mSlaveManager == mgr);
		mSlaveManager = 0;

	}
}

bool TopLevelSerializableUnitBase::updateManagerType(SerializeManager *mgr, bool isMaster)
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

