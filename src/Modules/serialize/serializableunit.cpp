#include "../moduleslib.h"
#include "serializableunit.h"

#include "serializemanager.h"

#include "toplevelserializableunit.h"

#include "streams/bufferedstream.h"

#include "streams/operations.h"

namespace Engine {
namespace Serialize {

    SerializableUnitBase::SerializableUnitBase(size_t masterId)
        : mMasterId(SerializeManager::generateMasterId(masterId, this))
    {
    }

    SerializableUnitBase::SerializableUnitBase(const SerializableUnitBase &other)
        : SerializableUnitBase()
    {
    }

    SerializableUnitBase::SerializableUnitBase(SerializableUnitBase &&other) noexcept
        : SerializableUnitBase()
    {
    }

    SerializableUnitBase::~SerializableUnitBase()
    {
        //If this fails, you forgot to add all Serializables to the SerializeTable.
        //Temporary solution! Proper solution: Move mSynced into Serializable (=> evtl default mActive to true)
        assert(!mSynced);

        if (!mSlaveId)
            static auto &log = LOG_ERROR("Fix SerializableUnitBase::clearSlaveId calls!");
        SerializeManager::deleteMasterId(mMasterId, this);
    }

    void SerializableUnitBase::writeState(SerializeOutStream &out, const char *name, StateTransmissionFlags flags) const
    {
        if (out.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            out.format().beginExtended(out, name, 1);
            write(out, mMasterId, "id");
        }
        out.format().beginCompound(out, name);
        mType->writeState(this, out);
        out.format().endCompound(out, name);
    }

    void SerializableUnitBase::readState(SerializeInStream &in, const char *name, StateTransmissionFlags flags)
    {
        if (!in.isMaster() && !(flags & StateTransmissionFlags_SkipId)) {
            in.format().beginExtended(in, name, 1);
            UnitId id;
            read(in, id, "id");

            if (in.manager() && in.manager()->getSlaveStreambuf() == &in.buffer()) {
                setSlaveId(id, in.manager());
                if (mType->mIsTopLevelUnit) {
                    static_cast<TopLevelSerializableUnitBase *>(this)->setStaticSlaveId(id);
                }
            }
        }
        in.format().beginCompound(in, name);
        mType->readState(this, in, flags);
        in.format().endCompound(in, name);
    }

    void SerializableUnitBase::readAction(BufferedInOutStream &in, PendingRequest *request)
    {
        mType->readAction(this, in, request);
    }

    void SerializableUnitBase::readRequest(BufferedInOutStream &in, TransactionId id)
    {
        mType->readRequest(this, in, id);
    }

    std::set<BufferedOutStream *, CompareStreamId> SerializableUnitBase::getMasterMessageTargets() const
    {
        std::set<BufferedOutStream *, CompareStreamId> result;
        if (mSynced) {
            result = topLevel()->getMasterMessageTargets();
        }
        return result;
    }

    BufferedOutStream *SerializableUnitBase::getSlaveMessageTarget() const
    {
        BufferedOutStream *result = nullptr;
        if (mSynced) {
            result = topLevel()->getSlaveMessageTarget();
        }
        return result;
    }

    const TopLevelSerializableUnitBase *SerializableUnitBase::topLevel() const
    {
        if (mType->mIsTopLevelUnit)
            return static_cast<const TopLevelSerializableUnitBase *>(this);
        else if (mParent)
            return mParent->topLevel();
        else
            return nullptr;
    }

    void SerializableUnitBase::clearSlaveId(SerializeManager *mgr)
    {
        if (mSlaveId != 0) {
            mgr->removeSlaveMapping(this);
            mSlaveId = 0;
        }
    }

    void SerializableUnitBase::setParent(SerializableUnitBase *parent)
    {
        mParent = parent;
    }

    UnitId SerializableUnitBase::slaveId() const
    {
        return mSlaveId;
    }

    UnitId SerializableUnitBase::masterId() const
    {
        return mMasterId;
    }

    void SerializableUnitBase::setSlaveId(UnitId id, SerializeManager *mgr)
    {
        if (mSlaveId != id) {
            if (mSlaveId != 0) {
                mgr->removeSlaveMapping(this);
            }
            mSlaveId = id;
            mgr->addSlaveMapping(this);
        }
    }

    void SerializableUnitBase::applySerializableMap(SerializeInStream &in)
    {
        mType->applySerializableMap(this, in);
    }

    void SerializableUnitBase::setSynced(bool b)
    {
        setDataSynced(b);
        setActive(b, true);
    }

    void SerializableUnitBase::setDataSynced(bool b)
    {
        assert(mSynced != b);
        mSynced = b;
        mType->setDataSynced(this, b);
    }

    void SerializableUnitBase::setActive(bool active, bool existenceChanged)
    {
        //assert(mSynced == active);
        mType->setActive(this, active, existenceChanged);
    }

    bool SerializableUnitBase::isActive(size_t index) const
    {
        return index < mActiveIndex;
    }

    void SerializableUnitBase::sync()
    {
        setSynced(true);
    }

    void SerializableUnitBase::unsync()
    {
        setSynced(false);
    }

    bool SerializableUnitBase::isSynced() const
    {
        return mSynced;
    }

    bool SerializableUnitBase::isMaster() const
    {
        return mSlaveId == 0;
    }

    const SerializeTable *SerializableUnitBase::serializeType() const
    {
        return mType;
    }

    UnitId SerializableUnitBase::moveMasterId(UnitId newId)
    {
        UnitId oldId = mMasterId;
        SerializeManager::deleteMasterId(mMasterId, this);
        mMasterId = SerializeManager::generateMasterId(newId, this);
        return oldId;
    }

} // namespace Serialize
} // namespace Core
