#include "../metalib.h"

#include "streams/bufferedstream.h"
#include "streams/operations.h"
#include "syncable.h"
#include "syncableunit.h"
#include "toplevelunit.h"

namespace Engine {
namespace Serialize {

    std::set<BufferedOutStream *, CompareStreamId> SyncableBase::getMasterActionMessageTargets(const SyncableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId,
        const std::set<ParticipantId> &targets) const
    {
        std::set<BufferedOutStream *, CompareStreamId> result = parent->getMasterMessageTargets();
        if (targets.empty()) {
            for (BufferedOutStream *out : result) {
                out->beginMessage(parent, ACTION, out->id() == answerTarget ? answerId : 0);
                *out << index;
            }
        } else {
            auto it1 = result.begin();
            auto it2 = targets.begin();
            while (it1 != result.end() && it2 != targets.end()) {
                BufferedOutStream *out = *it1;
                while (*it2 < out->id()) {
                    throw 0; //LOG_WARNING("Specific Target not in MessageTargetList!");
                    ++it2;
                }
                if (*it2 == out->id()) {
                    out->beginMessage(parent, ACTION, out->id() == answerTarget ? answerId : 0);
                    *out << index;
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

    BufferedOutStream &SyncableBase::getMasterRequestResponseTarget(const SyncableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId) const
    {
        for (BufferedOutStream *out : parent->getMasterMessageTargets()) {
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

    void SyncableBase::writeAction(const SyncableUnitBase *parent, uint8_t index, const void *data, ParticipantId answerTarget, TransactionId answerId, const std::set<ParticipantId> &targets) const
    {
        parent->serializeType()->writeAction(parent, index, getMasterActionMessageTargets(parent, index, answerTarget, answerId, targets), data);
    }

    BufferedOutStream &SyncableBase::getSlaveActionMessageTarget(const SyncableUnitBase *parent, uint8_t index, ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback) const
    {
        BufferedOutStream &out = parent->getSlaveMessageTarget();
        out.beginMessage(parent, REQUEST, out.createRequest(requester, requesterTransactionId, std::move(callback)));
        out << index;
        return out;
    }

    void SyncableBase::writeRequest(const SyncableUnitBase *parent, uint8_t index, const void *data, ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback) const
    {
        parent->serializeType()->writeRequest(parent, index, getSlaveActionMessageTarget(parent, index, requester, requesterTransactionId, std::move(callback)), data);
    }

    void SyncableBase::writeRequestResponse(const SyncableUnitBase *parent, uint8_t index, const void *data, ParticipantId answerTarget, TransactionId answerId) const
    {
        if (answerTarget != 0) {
            parent->serializeType()->writeAction(parent, index, { &getMasterRequestResponseTarget(parent, index, answerTarget, answerId) }, data);
        }
    }

    void SyncableBase::beginRequestResponseMessage(const SyncableUnitBase *parent, uint8_t index, BufferedOutStream &stream, TransactionId id) const
    {
        stream.beginMessage(parent, ACTION, id);
        stream << index;
    }

    bool SyncableBase::isMaster(const SyncableUnitBase *parent) const
    {
        return !parent->mSynced || !parent->mTopLevel || parent->mTopLevel->isMaster();
    }

}
}
