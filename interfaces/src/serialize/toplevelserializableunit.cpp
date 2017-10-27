#include "interfaceslib.h"
#include "toplevelserializableunit.h"

#include "streams/serializestream.h"


#include "serializemanager.h"

namespace Engine {
	namespace Serialize {

		TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(size_t staticId) :
			SerializableUnitBase(staticId),
			mSlaveManager(0),
			mStaticSlaveId(staticId)
		{
		}

		TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase & other) :
			SerializableUnitBase(other),
			mSlaveManager(0),
			mStaticSlaveId(other.mStaticSlaveId)
		{
		}

		TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(TopLevelSerializableUnitBase && other) :
			SerializableUnitBase(std::forward<TopLevelSerializableUnitBase>(other)),
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

		bool TopLevelSerializableUnitBase::isMaster() const
		{
			return mSlaveManager == 0;
		}

		ParticipantId TopLevelSerializableUnitBase::getLocalMasterParticipantId()
		{
			return SerializeManager::getLocalMasterParticipantId();
		}

		ParticipantId TopLevelSerializableUnitBase::getSlaveParticipantId() const
		{
			return mSlaveManager->getSlaveParticipantId();
		}

		void TopLevelSerializableUnitBase::setStaticSlaveId(size_t staticId)
		{
			mStaticSlaveId = staticId;
		}

		void TopLevelSerializableUnitBase::initSlaveId()
		{
			setSlaveId(mStaticSlaveId ? mStaticSlaveId : mMasterId.first);
		}

		size_t TopLevelSerializableUnitBase::readId(SerializeInStream & in)
		{
			mStaticSlaveId = SerializableUnitBase::readId(in);
			return mStaticSlaveId;
		}

		std::set<BufferedOutStream*, CompareStreamId> TopLevelSerializableUnitBase::getMasterMessageTargets()
		{
			std::set<BufferedOutStream*, CompareStreamId> result;
			for (SerializeManager *mgr : mMasterManagers) {
				const std::set<BufferedOutStream*, CompareStreamId> &targets = mgr->getMasterMessageTargets();
				std::set<BufferedOutStream*, CompareStreamId> temp;
				std::set_union(result.begin(), result.end(), targets.begin(), targets.end(), std::inserter(temp, temp.begin()), CompareStreamId{});
				temp.swap(result);
			}
			return result;
		}

	BufferedOutStream * TopLevelSerializableUnitBase::getSlaveMessageTarget() const
	{
		if (mSlaveManager) {
			return mSlaveManager->getSlaveMessageTarget();
		}
		return nullptr;
	}

	const std::list<SerializeManager*>& TopLevelSerializableUnitBase::getMasterManagers() const
	{
		return mMasterManagers;
	}

	SerializeManager * TopLevelSerializableUnitBase::getSlaveManager() const
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

const TopLevelSerializableUnitBase * TopLevelSerializableUnitBase::topLevel() const
{
	return this;
}

} // namespace Serialize
} // namespace Core

