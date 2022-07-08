#include "../../metalib.h"

#include "syncableunit.h"

#include "statetransmissionflags.h"

#include "serializetable.h"

#include "toplevelunit.h"

#include "../syncmanager.h"

#include "../operations.h"

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

    void SyncableUnitBase::writeState(FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy, StateTransmissionFlags flags) const
    {
        if (out.isMaster(AccessMode::WRITE) && out.data() && !(flags & StateTransmissionFlags_SkipId)) {
            out.beginExtendedWrite(name, 1);
            write(out, mMasterId, "syncId");
        }
        customUnitPtr().writeState(out, name, hierarchy, flags | StateTransmissionFlags_SkipId);
    }

    StreamResult SyncableUnitBase::readState(FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy, StateTransmissionFlags flags)
    {
        if (!in.isMaster(AccessMode::READ) && in.data() && !(flags & StateTransmissionFlags_SkipId)) {
            STREAM_PROPAGATE_ERROR(in.beginExtendedRead(name, 1));
            UnitId id;
            STREAM_PROPAGATE_ERROR(read(in, id, "syncId"));

            if (in.manager() && in.manager()->getSlaveStreamData() == in.data()) {
                setSlaveId(id, in.manager());
                if (mType->mIsTopLevelUnit) {
                    static_cast<TopLevelUnitBase *>(this)->setStaticSlaveId(id);
                }
            }
        }
        return customUnitPtr().readState(in, name, hierarchy, flags | StateTransmissionFlags_SkipId);
    }

    StreamResult SyncableUnitBase::applyMap(FormattedSerializeStream &in, bool success, CallerHierarchyBasePtr hierarchy)
    {
        return mType->applyMap(this, in, success, hierarchy);
    }

    void SyncableUnitBase::setActive(bool active, bool existenceChanged)
    {
        mType->setActive(this, active, existenceChanged);
    }

    StreamResult SyncableUnitBase::readAction(FormattedBufferedStream &in, PendingRequest &request)
    {
        return mType->readAction(this, in, request);
    }

    StreamResult SyncableUnitBase::readRequest(FormattedBufferedStream &in, MessageId id)
    {
        return mType->readRequest(this, in, id);
    }

    std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> SyncableUnitBase::getMasterMessageTargets(const std::set<ParticipantId> &targets) const
    {
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> result;
        if (mSynced) {
            result = mTopLevel->getMasterMessageTargets();
            if (!targets.empty()) {
                std::erase_if(result,
                    [&](FormattedBufferedStream &stream) { return !targets.contains(stream.id()); });
            }
        }
        return result;
    }

    FormattedBufferedStream &SyncableUnitBase::getMasterRequestResponseTarget(ParticipantId answerTarget) const
    {
        for (FormattedBufferedStream &out : getMasterMessageTargets()) {
            if (out.id() == answerTarget) {
                return out;
            }
        }
        throw 0;
    }

    FormattedBufferedStream &SyncableUnitBase::getSlaveMessageTarget() const
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

    void SyncableUnitBase::writeFunctionAction(uint16_t index, const void *args, const std::set<ParticipantId> &targets, ParticipantId answerTarget, MessageId answerId)
    {
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> outStreams = getMasterMessageTargets(targets);
        for (FormattedBufferedStream &out : outStreams) {
            out.beginMessageWrite();
            SyncManager::writeActionHeader(out, this, MessageType::FUNCTION_ACTION, out.id() == answerTarget ? answerId : 0);
        }
        mType->writeFunctionArguments(outStreams, index, CALL, args);
    }

    void SyncableUnitBase::writeFunctionResult(uint16_t index, const void *result, ParticipantId answerTarget, MessageId answerId)
    {
        assert(answerTarget != 0 && answerId != 0);
        FormattedBufferedStream &out = getMasterRequestResponseTarget(answerTarget);
        out.beginMessageWrite();
        SyncManager::writeActionHeader(out, this, MessageType::FUNCTION_ACTION, answerId);
        mType->writeFunctionResult(out, index, result);
    }

    void SyncableUnitBase::writeFunctionRequest(uint16_t index, FunctionType type, const void *args, ParticipantId requester, MessageId requesterTransactionId, GenericMessagePromise promise)
    {
        FormattedBufferedStream &out = getSlaveMessageTarget();
        out.beginMessageWrite(requester, requesterTransactionId, std::move(promise));
        SyncManager::writeHeader(out, this, MessageType::FUNCTION_REQUEST);
        mType->writeFunctionArguments({ out }, index, type, args);
    }

    StreamResult SyncableUnitBase::readFunctionAction(FormattedBufferedStream &in, PendingRequest &request)
    {
        return mType->readFunctionAction(this, in, request);
    }

    StreamResult SyncableUnitBase::readFunctionRequest(FormattedBufferedStream &in, MessageId id)
    {
        return mType->readFunctionRequest(this, in, id);
    }

}
}