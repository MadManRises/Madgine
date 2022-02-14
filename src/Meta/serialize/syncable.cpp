#include "../metalib.h"

#include "serializetable.h"
#include "streams/formattedbufferedstream.h"
#include "streams/operations.h"
#include "syncable.h"
#include "syncableunit.h"
#include "toplevelunit.h"

namespace Engine {
namespace Serialize {

    std::set<FormattedBufferedStream *, CompareStreamId> SyncableBase::getMasterActionMessageTargets(const SyncableUnitBase *parent, OffsetPtr offset, ParticipantId answerTarget, TransactionId answerId,
        const std::set<ParticipantId> &targets) const
    {
        return getMasterActionMessageTargets(parent, parent->serializeType()->getIndex(offset), answerTarget, answerId, targets);
    }

    std::set<FormattedBufferedStream *, CompareStreamId> SyncableBase::getMasterActionMessageTargets(const SyncableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId,
        const std::set<ParticipantId> &targets) const
    {
        std::set<FormattedBufferedStream *, CompareStreamId> result = parent->getMasterMessageTargets();
        if (targets.empty()) {
            for (FormattedBufferedStream *out : result) {
                out->beginMessage(parent, ACTION, out->id() == answerTarget ? answerId : 0);
                write(*out, index, "index");
            }
        } else {
            auto it1 = result.begin();
            auto it2 = targets.begin();
            while (it1 != result.end() && it2 != targets.end()) {
                FormattedBufferedStream *out = *it1;
                while (*it2 < out->id()) {
                    throw 0; //LOG_WARNING("Specific Target not in MessageTargetList!");
                    ++it2;
                }
                if (*it2 == out->id()) {
                    out->beginMessage(parent, ACTION, out->id() == answerTarget ? answerId : 0);
                    write(*out, index, "index");
                    ++it2;
                    ++it1;
                } else {
                    it1 = result.erase(it1);
                }
            }
            result.erase(it1, result.end());
        }

        return result;
    }

    FormattedBufferedStream &SyncableBase::getMasterRequestResponseTarget(const SyncableUnitBase *parent, OffsetPtr offset, ParticipantId answerTarget, TransactionId answerId) const
    {
        return getMasterRequestResponseTarget(parent, parent->serializeType()->getIndex(offset), answerTarget, answerId);
    }

    FormattedBufferedStream &SyncableBase::getMasterRequestResponseTarget(const SyncableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId) const
    {
        for (FormattedBufferedStream *out : parent->getMasterMessageTargets()) {
            if (out->id() == answerTarget) {
                beginRequestResponseMessage(parent, index, *out, answerId);
                return *out;
            }
        }
        throw 0;
    }

    ParticipantId SyncableBase::participantId(const SerializableUnitBase *parent)
    {
        return parent->mTopLevel->participantId();
    }

    void SyncableBase::writeAction(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId answerTarget, TransactionId answerId, const std::set<ParticipantId> &targets) const
    {
        uint8_t index = parent->serializeType()->getIndex(offset);
        parent->serializeType()->writeAction(parent, index, getMasterActionMessageTargets(parent, index, answerTarget, answerId, targets), data);
    }

    FormattedBufferedStream &SyncableBase::getSlaveActionMessageTarget(const SyncableUnitBase *parent, OffsetPtr offset, ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback) const
    {
        return getSlaveActionMessageTarget(parent, parent->serializeType()->getIndex(offset), requester, requesterTransactionId, callback);
    }

    FormattedBufferedStream &SyncableBase::getSlaveActionMessageTarget(const SyncableUnitBase *parent, uint8_t index, ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback) const
    {
        FormattedBufferedStream &out = parent->getSlaveMessageTarget();
        out.beginMessage(parent, REQUEST, out.createRequest(requester, requesterTransactionId, std::move(callback)));
        write(out, index, "index");
        return out;
    }

    void SyncableBase::writeRequest(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback) const
    {
        uint8_t index = parent->serializeType()->getIndex(offset);
        parent->serializeType()->writeRequest(parent, index, getSlaveActionMessageTarget(parent, index, requester, requesterTransactionId, std::move(callback)), data);
    }

    void SyncableBase::writeRequestResponse(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId answerTarget, TransactionId answerId) const
    {
        if (answerTarget != 0) {
            uint8_t index = parent->serializeType()->getIndex(offset);
            parent->serializeType()->writeAction(parent, index, { &getMasterRequestResponseTarget(parent, index, answerTarget, answerId) }, data);
        }
    }

    void SyncableBase::beginRequestResponseMessage(const SyncableUnitBase *parent, OffsetPtr offset, FormattedBufferedStream &stream, TransactionId id) const
    {
        beginRequestResponseMessage(parent, parent->serializeType()->getIndex(offset), stream, id);
    }

    void SyncableBase::beginRequestResponseMessage(const SyncableUnitBase *parent, uint8_t index, FormattedBufferedStream &stream, TransactionId id) const
    {
        stream.beginMessage(parent, ACTION, id);
        write(stream, index, "index");
    }

    bool SyncableBase::isMaster(const SyncableUnitBase *parent) const
    {
        return !parent->mSynced || !parent->mTopLevel || parent->mTopLevel->isMaster();
    }

}
}
