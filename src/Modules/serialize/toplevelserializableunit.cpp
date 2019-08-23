#include "../moduleslib.h"
#include "toplevelserializableunit.h"

#include "streams/serializestream.h"

#include "serializemanager.h"

namespace Engine {
namespace Serialize {
    TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(size_t staticId)
        : SerializableUnitBase(staticId)
        , mSlaveManager(nullptr)
        , mStaticSlaveId(staticId)
    {
        assert(staticId == 0 || (staticId >= BEGIN_STATIC_ID_SPACE && staticId < RESERVED_ID_COUNT));
    }

    TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase &other)
        : SerializableUnitBase(other.masterId())
        , mSlaveManager(nullptr)
        , mStaticSlaveId(other.mStaticSlaveId)
    {
    }

    TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(TopLevelSerializableUnitBase &&other) noexcept
        : SerializableUnitBase(other.masterId())
        , mSlaveManager(nullptr)
        , mStaticSlaveId(other.mStaticSlaveId)
    {
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

    void TopLevelSerializableUnitBase::copyStaticSlaveId(const TopLevelSerializableUnitBase &other)
    {
        mStaticSlaveId = other.mStaticSlaveId;
    }

    /*bool TopLevelSerializableUnitBase::isMaster() const
		{
			return mSlaveManager == nullptr;
		}*/

    ParticipantId TopLevelSerializableUnitBase::participantId() const
    {
        return SerializeManager::getParticipantId(mSlaveManager);
    }

    void TopLevelSerializableUnitBase::setStaticSlaveId(
        size_t staticId)
    {
        assert(staticId == 0 || (staticId >= BEGIN_STATIC_ID_SPACE && staticId < RESERVED_ID_COUNT));
        mStaticSlaveId = staticId;
    }

    void TopLevelSerializableUnitBase::initSlaveId()
    {
        setSlaveId(mStaticSlaveId ? mStaticSlaveId : masterId());
    }

    std::set<BufferedOutStream *, CompareStreamId> TopLevelSerializableUnitBase::getMasterMessageTargets() const
    {
        std::set<BufferedOutStream *, CompareStreamId> result;
        for (SerializeManager *mgr : mMasterManagers) {
            const std::set<BufferedOutStream *, CompareStreamId> &targets = mgr->getMasterMessageTargets();
            std::set<BufferedOutStream *, CompareStreamId> temp;
            set_union(result.begin(), result.end(), targets.begin(), targets.end(), inserter(temp, temp.begin()),
                CompareStreamId {});
            temp.swap(result);
        }
        return result;
    }

    BufferedOutStream *TopLevelSerializableUnitBase::getSlaveMessageTarget() const
    {
        if (mSlaveManager) {
            return mSlaveManager->getSlaveMessageTarget();
        }
        return nullptr;
    }

    const std::vector<SerializeManager *> &TopLevelSerializableUnitBase::getMasterManagers() const
    {
        return mMasterManagers;
    }

    SerializeManager *TopLevelSerializableUnitBase::getSlaveManager() const
    {
        return mSlaveManager;
    }

    bool TopLevelSerializableUnitBase::addManager(SerializeManager *mgr)
    {
        if (mgr->isMaster()) {
            mMasterManagers.push_back(mgr);
        } else {
            if (mSlaveManager)
                return false;
            mSlaveManager = mgr;
        }
        return true;
    }

    void TopLevelSerializableUnitBase::removeManager(SerializeManager *mgr)
    {
        if (mgr->isMaster()) {
            mMasterManagers.erase(std::remove(mMasterManagers.begin(), mMasterManagers.end(), mgr), mMasterManagers.end());
        } else {
            assert(mSlaveManager == mgr);
            mSlaveManager = nullptr;
        }
    }

    bool TopLevelSerializableUnitBase::updateManagerType(SerializeManager *mgr, bool isMaster)
    {
        if (isMaster) {
            removeManager(mgr);
            mMasterManagers.push_back(mgr);
        } else {
            if (mSlaveManager)
                return false;
            removeManager(mgr);
            mSlaveManager = mgr;
        }
        return true;
    }

} // namespace Serialize
} // namespace Core
