#include "../metalib.h"

#include "syncableunit.h"

#include "syncmanager.h"

#include "streams/serializestream.h"

#include "formatter.h"

#include "streams/operations.h"

#include "toplevelunit.h"

#include "serializableunitptr.h"

namespace Engine {
namespace Serialize {

    SyncableUnitBase::SyncableUnitBase(UnitId masterId)
        : mMasterId(SerializeManager::generateMasterId(masterId, this))
    {
    }

    SyncableUnitBase::SyncableUnitBase(const SyncableUnitBase &other)
        : SerializableUnitBase(other)
        , mMasterId(SerializeManager::generateMasterId(0, this))
    {
    }

    SyncableUnitBase::SyncableUnitBase(SyncableUnitBase &&other) noexcept
        : SerializableUnitBase(std::move(other))
        , mSlaveId(std::exchange(other.mSlaveId, 0))
        , mMasterId(SerializeManager::updateMasterId(std::exchange(other.mMasterId, SerializeManager::generateMasterId(0, &other)), this))
    {
    }

    SyncableUnitBase::~SyncableUnitBase()
    {
        assert(!mSlaveId);            
        SerializeManager::deleteMasterId(mMasterId, this);
    }

    SyncableUnitBase &SyncableUnitBase::operator=(const SyncableUnitBase &other)
    {
        SerializableUnitBase::operator=(other);
        return *this;
    }

    SyncableUnitBase &SyncableUnitBase::operator=(SyncableUnitBase &&other)
    {
        SerializableUnitBase::operator=(std::move(other));
        mSlaveId = std::exchange(other.mSlaveId, 0);
        std::swap(mMasterId, other.mMasterId);
        SerializeManager::updateMasterId(mMasterId, this);
        SerializeManager::updateMasterId(other.mMasterId, &other);
        return *this;
    }

    void SyncableUnitBase::writeState(SerializeOutStream &out, const char *name, CallerHierarchyBasePtr hierarchy, StateTransmissionFlags flags) const
    {
        if (out.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            out.format().beginExtended(out, name, 1);
            write(out, mMasterId, "syncId");
        }
        SerializableUnitConstPtr { this, mType }.writeState(out, name, hierarchy, flags | StateTransmissionFlags_SkipId);
    }

    StreamResult SyncableUnitBase::readState(SerializeInStream &in, const char *name, CallerHierarchyBasePtr hierarchy, StateTransmissionFlags flags)
    {
        if (!in.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            STREAM_PROPAGATE_ERROR(in.format().beginExtended(in, name, 1));
            UnitId id;
            STREAM_PROPAGATE_ERROR(read(in, id, "syncId"));

            if (in.manager() && in.manager()->getSlaveStreamData() == &in.data()) {
                setSlaveId(id, in.manager());
                if (serializeType()->mIsTopLevelUnit) {
                    static_cast<TopLevelUnitBase *>(this)->setStaticSlaveId(id);
                }
            }
        }
        return SerializableUnitPtr { this, mType }.readState(in, name, hierarchy, flags | StateTransmissionFlags_SkipId);
    }

    StreamResult SyncableUnitBase::readAction(BufferedInOutStream &in, PendingRequest *request)
    {
        return serializeType()->readAction(this, in, request);
    }

    StreamResult SyncableUnitBase::readRequest(BufferedInOutStream &in, TransactionId id)
    {
        return serializeType()->readRequest(this, in, id);
    }

    std::set<BufferedOutStream *, CompareStreamId> SyncableUnitBase::getMasterMessageTargets() const
    {
        std::set<BufferedOutStream *, CompareStreamId> result;
        if (mSynced) {
            result = mTopLevel->getMasterMessageTargets();
        }
        return result;
    }

    BufferedOutStream &SyncableUnitBase::getSlaveMessageTarget() const
    {
        assert(mSynced);
        return mTopLevel->getSlaveMessageTarget();
    }

    void SyncableUnitBase::clearSlaveId(SerializeManager *mgr)
    {
        if (mSlaveId != 0) {
            mgr->removeSlaveMapping(this);
            mSlaveId = 0;
        }
    }

    SerializableUnitPtr SyncableUnitBase::customUnitPtr()
    {
        return { this, mType };
    }

    SerializableUnitConstPtr SyncableUnitBase::customUnitPtr() const
    {
        return { this, mType };
    }

    UnitId SyncableUnitBase::slaveId() const
    {
        return mSlaveId;
    }

    UnitId SyncableUnitBase::masterId() const
    {
        return mMasterId;
    }

    void SyncableUnitBase::setSlaveId(UnitId id, SerializeManager *mgr)
    {
        if (mTopLevel->getSlaveManager() != mgr) {
            assert(!mTopLevel->getSlaveManager());
            assert(mSlaveId == 0);
        }
        if (mSlaveId != id) {
            if (mSlaveId != 0) {
                mgr->removeSlaveMapping(this);
            }
            if (mTopLevel->getSlaveManager() == mgr)    
                mSlaveId = id;
            mgr->addSlaveMapping(id, this);
        }
    }

    bool SyncableUnitBase::isMaster() const
    {
        return mSlaveId == 0;
    }

    UnitId SyncableUnitBase::moveMasterId(UnitId newId)
    {
        UnitId oldId = mMasterId;
        SerializeManager::deleteMasterId(mMasterId, this);
        mMasterId = SerializeManager::generateMasterId(newId, this);
        return oldId;
    }

    const SerializeTable *SyncableUnitBase::serializeType() const
    {
        return mType;
    }

}
}