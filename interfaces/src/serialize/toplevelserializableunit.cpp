#include "interfaceslib.h"
#include "toplevelserializableunit.h"

#include "streams/serializestream.h"


#include "serializemanager.h"

namespace Engine
{
	namespace Serialize
	{
		TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(size_t staticId) :
			SerializableUnitBase(staticId),
			mSlaveManager(nullptr),
			mStaticSlaveId(staticId)
		{
		}

		TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase& other) :
			SerializableUnitBase(other),
			mSlaveManager(nullptr),
			mStaticSlaveId(other.mStaticSlaveId)
		{
		}

		TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(TopLevelSerializableUnitBase&& other) noexcept :
			SerializableUnitBase(std::forward<TopLevelSerializableUnitBase>(other)),
			mSlaveManager(nullptr),
			mStaticSlaveId(other.mStaticSlaveId)
		{
			while (!other.mMasterManagers.empty())
			{
				other.mMasterManagers.front()->moveTopLevelItem(&other, this);
			}
			if (other.mSlaveManager)
			{
				other.mSlaveManager->moveTopLevelItem(&other, this);
			}
		}


		TopLevelSerializableUnitBase::~TopLevelSerializableUnitBase()
		{
			if (mSlaveManager)
			{
				mSlaveManager->removeTopLevelItem(this);
			}
			while (!mMasterManagers.empty())
			{
				mMasterManagers.front()->removeTopLevelItem(this);
			}
		}

		void TopLevelSerializableUnitBase::copyStaticSlaveId(const TopLevelSerializableUnitBase& other)
		{
			mStaticSlaveId = other.mStaticSlaveId;
		}

		/*bool TopLevelSerializableUnitBase::isMaster() const
		{
			return mSlaveManager == nullptr;
		}*/

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

		size_t TopLevelSerializableUnitBase::readId(SerializeInStream& in)
		{
			mStaticSlaveId = SerializableUnitBase::readId(in);
			return mStaticSlaveId;
		}

		std::set<BufferedOutStream*, CompareStreamId> TopLevelSerializableUnitBase::getMasterMessageTargets()
		{
			std::set<BufferedOutStream*, CompareStreamId> result;
			for (SerializeManager* mgr : mMasterManagers)
			{
				const std::set<BufferedOutStream*, CompareStreamId>& targets = mgr->getMasterMessageTargets();
				std::set<BufferedOutStream*, CompareStreamId> temp;
				set_union(result.begin(), result.end(), targets.begin(), targets.end(), inserter(temp, temp.begin()),
				          CompareStreamId{});
				temp.swap(result);
			}
			return result;
		}

		BufferedOutStream* TopLevelSerializableUnitBase::getSlaveMessageTarget() const
		{
			if (mSlaveManager)
			{
				return mSlaveManager->getSlaveMessageTarget();
			}
			return nullptr;
		}

		const std::list<SerializeManager*>& TopLevelSerializableUnitBase::getMasterManagers() const
		{
			return mMasterManagers;
		}

		SerializeManager* TopLevelSerializableUnitBase::getSlaveManager() const
		{
			return mSlaveManager;
		}

		bool TopLevelSerializableUnitBase::addManager(SerializeManager* mgr)
		{
			if (mgr->isMaster())
			{
				mMasterManagers.push_back(mgr);
			}
			else
			{
				if (mSlaveManager)
					return false;
				mSlaveManager = mgr;
			}
			return true;
		}

		void TopLevelSerializableUnitBase::removeManager(SerializeManager* mgr)
		{
			if (mgr->isMaster())
			{
				size_t check = mMasterManagers.size();
				mMasterManagers.remove(mgr);
				assert(mMasterManagers.size() == check - 1);
			}
			else
			{
				assert(mSlaveManager == mgr);
				mSlaveManager = nullptr;
			}
		}

		bool TopLevelSerializableUnitBase::updateManagerType(SerializeManager* mgr, bool isMaster)
		{
			if (isMaster)
			{
				removeManager(mgr);
				mMasterManagers.push_back(mgr);
			}
			else
			{
				if (mSlaveManager)
					return false;
				removeManager(mgr);
				mSlaveManager = mgr;
			}
			return true;
		}

		const TopLevelSerializableUnitBase* TopLevelSerializableUnitBase::topLevel() const
		{
			return this;
		}
	} // namespace Serialize
} // namespace Core
