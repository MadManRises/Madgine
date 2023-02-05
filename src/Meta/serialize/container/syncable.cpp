#include "../../metalib.h"

#include "syncable.h"

#include "Generic/offsetptr.h"

#include "../hierarchy/serializetable.h"

#include "../hierarchy/toplevelunit.h"
#include "../syncmanager.h"

namespace Engine {
namespace Serialize {

    std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> SyncableBase::getMasterActionMessageTargets(const SyncableUnitBase *parent, ParticipantId answerTarget, MessageId answerId,
        const std::set<ParticipantId> &targets) const
    {
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> result = parent->getMasterMessageTargets();
        if (targets.empty()) {
            for (FormattedBufferedStream &out : result) {
                out.beginMessageWrite();
                SyncManager::writeActionHeader(out, parent, MessageType::ACTION, out.id() == answerTarget ? answerId : 0);
            }
        } else {
            auto it1 = result.begin();
            auto it2 = targets.begin();
            while (it1 != result.end() && it2 != targets.end()) {
                FormattedBufferedStream &out = *it1;
                while (*it2 < out.id()) {
                    throw 0; //LOG_WARNING("Specific Target not in MessageTargetList!");
                    ++it2;
                }
                if (*it2 == out.id()) {
                    out.beginMessageWrite();
                    SyncManager::writeActionHeader(out, parent, MessageType::ACTION, out.id() == answerTarget ? answerId : 0);
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

    FormattedBufferedStream &SyncableBase::getMasterRequestResponseTarget(const SyncableUnitBase *parent, ParticipantId answerTarget, MessageId answerId) const
    {
        FormattedBufferedStream &out = parent->getMasterRequestResponseTarget(answerTarget);
        beginRequestResponseMessage(parent, out, answerId);
        return out;
    }

    ParticipantId SyncableBase::participantId(const SerializableUnitBase *parent)
    {
        return parent->mTopLevel->participantId();
    }

    void SyncableBase::writeAction(const SyncableUnitBase *parent, OffsetPtr offset, void *data, ParticipantId answerTarget, MessageId answerId, const std::set<ParticipantId> &targets) const
    {
        uint16_t index = parent->serializeType()->getIndex(offset);
        parent->serializeType()->writeAction(parent, index, getMasterActionMessageTargets(parent, answerTarget, answerId, targets), data);
    }

    FormattedBufferedStream &SyncableBase::getSlaveRequestMessageTarget(const SyncableUnitBase *parent, ParticipantId requester, MessageId requesterTransactionId, GenericMessageReceiver receiver) const
    {
        FormattedBufferedStream &out = parent->getSlaveMessageTarget();
        out.beginMessageWrite(requester, requesterTransactionId, std::move(receiver));
        SyncManager::writeHeader(out, parent, MessageType::REQUEST);
        return out;
    }

    void SyncableBase::writeRequest(const SyncableUnitBase *parent, OffsetPtr offset, void *data, ParticipantId requester, MessageId requesterTransactionId, GenericMessageReceiver receiver) const
    {
        uint16_t index = parent->serializeType()->getIndex(offset);
        parent->serializeType()->writeRequest(parent, index, getSlaveRequestMessageTarget(parent, requester, requesterTransactionId, std::move(receiver)), data);
    }

    void SyncableBase::writeRequestResponse(const SyncableUnitBase *parent, OffsetPtr offset, void *data, ParticipantId answerTarget, MessageId answerId) const
    {
        if (answerTarget != 0) {
            uint16_t index = parent->serializeType()->getIndex(offset);
            parent->serializeType()->writeAction(parent, index, { getMasterRequestResponseTarget(parent, answerTarget, answerId) }, data);
        }
    }

    void SyncableBase::beginRequestResponseMessage(const SyncableUnitBase *parent, FormattedBufferedStream &stream, MessageId id) const
    {
        stream.beginMessageWrite();
        SyncManager::writeActionHeader(stream, parent, MessageType::ACTION, id);
    }

    bool SyncableBase::isMaster(const SyncableUnitBase *parent) const
    {
        return !parent->mSynced || !parent->mTopLevel || parent->mTopLevel->isMaster();
    }

}
}
