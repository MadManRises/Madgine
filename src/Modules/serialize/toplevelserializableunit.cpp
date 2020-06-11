#include "../moduleslib.h"
#include "toplevelserializableunit.h"

#include "syncmanager.h"

namespace Engine {
namespace Serialize {
    TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(UnitId staticId)
        : SerializableUnitBase(staticId)
        , mStaticSlaveId(staticId)
    {
        assert(staticId == 0 || (staticId >= BEGIN_STATIC_ID_SPACE && staticId < RESERVED_ID_COUNT));
    }

    TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase &other)
        : SerializableUnitBase()
        , mSlaveManager(nullptr)
        , mStaticSlaveId(other.mStaticSlaveId)
    {
    }

    TopLevelSerializableUnitBase::TopLevelSerializableUnitBase(TopLevelSerializableUnitBase &&other) noexcept
        : SerializableUnitBase(other.moveMasterId())
        , mStaticSlaveId(other.mStaticSlaveId)
    {
        if (other.mSlaveManager) {
            other.mSlaveManager->moveTopLevelItem(&other, this);
        }
        while (!other.mMasterManagers.empty()) {
            other.mMasterManagers.front()->moveTopLevelItem(&other, this);
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

    ParticipantId TopLevelSerializableUnitBase::participantId() const
    {
        return SyncManager::getParticipantId(mSlaveManager);
    }

    void TopLevelSerializableUnitBase::setStaticSlaveId(
        UnitId staticId)
    {
        assert(staticId == 0 || (staticId >= BEGIN_STATIC_ID_SPACE /* && staticId < RESERVED_ID_COUNT*/));
        mStaticSlaveId = staticId;
    }

    void TopLevelSerializableUnitBase::initSlaveId(SerializeManager *mgr)
    {
        setSlaveId(mStaticSlaveId ? mStaticSlaveId : masterId(), mgr);
    }

    std::set<BufferedOutStream *, CompareStreamId> TopLevelSerializableUnitBase::getMasterMessageTargets() const
    {
        std::set<BufferedOutStream *, CompareStreamId> result;
        for (SyncManager *mgr : mMasterManagers) {
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

    const std::vector<SyncManager *> &TopLevelSerializableUnitBase::getMasterManagers() const
    {
        return mMasterManagers;
    }

    SyncManager *TopLevelSerializableUnitBase::getSlaveManager() const
    {
        return mSlaveManager;
    }

    bool TopLevelSerializableUnitBase::addManager(SyncManager *mgr)
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

    void TopLevelSerializableUnitBase::removeManager(SyncManager *mgr)
    {
        if (mgr->isMaster()) {
            mMasterManagers.erase(std::remove(mMasterManagers.begin(), mMasterManagers.end(), mgr), mMasterManagers.end());
        } else {
            assert(mSlaveManager == mgr);
            mSlaveManager = nullptr;
        }
    }

    bool TopLevelSerializableUnitBase::updateManagerType(SyncManager *mgr, bool isMaster)
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
