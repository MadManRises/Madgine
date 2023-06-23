#include "../../metalib.h"

#include "toplevelunit.h"

#include "serializableids.h"

#include "../syncmanager.h"

#include "serializableunitptr.h"

namespace Engine {
namespace Serialize {
    TopLevelUnitBase::TopLevelUnitBase(UnitId staticId)
        : SyncableUnitBase(staticId)
        , mStaticSlaveId(staticId) 
    {
        assert(staticId == 0 || (staticId >= BEGIN_STATIC_ID_SPACE && staticId < RESERVED_ID_COUNT));
        mTopLevel = this;
    }

    TopLevelUnitBase::TopLevelUnitBase(const TopLevelUnitBase &other)
        : SyncableUnitBase()
        , mSlaveManager(nullptr)
        , mStaticSlaveId(other.mStaticSlaveId)
    {
    }

    TopLevelUnitBase::TopLevelUnitBase(TopLevelUnitBase &&other) noexcept
        : SyncableUnitBase(other.moveMasterId())
        , mStaticSlaveId(other.mStaticSlaveId)
    {
        while (!other.mManagers.empty()) {
            other.mManagers.front()->moveTopLevelItem(&other, this);
        }
    }

    TopLevelUnitBase::~TopLevelUnitBase()
    {
        while (!mManagers.empty()) {
            mManagers.front()->removeTopLevelItem(this);
        }
    }

    void TopLevelUnitBase::sync()
    {
        SerializableUnitPtr self { this, serializeType() };
        self.setSynced(true);
        self.setActive(true, true);
    }

    void TopLevelUnitBase::unsync()
    {
        SerializableUnitPtr self { this, serializeType() };
        self.setSynced(false);
        self.setActive(false, true);
    }

    ParticipantId TopLevelUnitBase::participantId() const
    {
        return SyncManager::getParticipantId(mSlaveManager);
    }

    void TopLevelUnitBase::setStaticSlaveId(
        UnitId staticId)
    {
        assert(staticId == 0 || (staticId >= BEGIN_STATIC_ID_SPACE /* && staticId < RESERVED_ID_COUNT*/));
        mStaticSlaveId = staticId;
    }

    void TopLevelUnitBase::initSlaveId(SyncManager *mgr)
    {
        if (mStaticSlaveId)
            setSlaveId(mStaticSlaveId, mgr);                
    }

    std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> TopLevelUnitBase::getMasterMessageTargets() const
    {
        //TODO: maybe return std::vector
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> result;
        for (SyncManager *mgr : mManagers) {
            const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &targets = mgr->getMasterMessageTargets();
            std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> temp;
            set_union(result.begin(), result.end(), targets.begin(), targets.end(), inserter(temp, temp.begin()),
                CompareStreamId {});
            temp.swap(result);
        }
        return result;
    }

    FormattedBufferedStream &TopLevelUnitBase::getSlaveMessageTarget() const
    {
        assert(mSlaveManager);
        return mSlaveManager->getSlaveMessageTarget();
    }

    const std::vector<SyncManager *> &TopLevelUnitBase::getManagers() const
    {
        return mManagers;
    }

    SyncManager *TopLevelUnitBase::getSlaveManager() const
    {
        return mSlaveManager;
    }

    bool TopLevelUnitBase::addManager(SyncManager *mgr)
    {
        if (!mgr->isMaster()) {
            if (mSlaveManager)
                return false;
            mSlaveManager = mgr;
        }
        mManagers.push_back(mgr);
        return true;
    }

    void TopLevelUnitBase::removeManager(SyncManager *mgr)
    {
        std::erase(mManagers, mgr);
        if (!mgr->isMaster()) {
            assert(mSlaveManager == mgr);
            mSlaveManager = nullptr;
        }
    }

    bool TopLevelUnitBase::updateManagerType(SyncManager *mgr, bool isMaster)
    {
        if (isMaster) {
            assert(mSlaveManager == mgr);
            mSlaveManager = nullptr;
        }else{
            if (mSlaveManager)
                return false;
            mSlaveManager = mgr;
        }
        return true;
    }

} // namespace Serialize
} // namespace Core
