#pragma once

#include "streams/comparestreamid.h"

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT SyncableBase {
        ~SyncableBase() = default;

        /*virtual void readRequest(BufferedInOutStream &in) = 0;
        virtual void readAction(SerializeInStream &in) = 0;*/

    protected:
        BufferedOutStream *getSlaveActionMessageTarget(const SerializableUnitBase *parent, uint8_t index, ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback) const;
        std::set<BufferedOutStream *, CompareStreamId> getMasterActionMessageTargets(const SerializableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId,
            const std::set<ParticipantId> &targets = {}) const;
        ParticipantId participantId(const SerializableUnitBase *parent);

        void beginActionResponseMessage(const SerializableUnitBase *parent, uint8_t index, BufferedOutStream *stream, TransactionId id) const;
        BufferedOutStream *beginActionResponseMessage(const SerializableUnitBase *parent, uint8_t index, ParticipantId stream, TransactionId id) const;

        bool isMaster(const SerializableUnitBase *parent) const;
    };

    template <typename OffsetPtr>
    struct Syncable : SyncableBase {
    protected:
        BufferedOutStream *getSlaveActionMessageTarget(ParticipantId requester = 0, TransactionId requesterTransactionId = 0, std::function<void(void *)> callback = {}) const
        {
            return SyncableBase::getSlaveActionMessageTarget(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableUnitBase>()), requester, requesterTransactionId, std::move(callback));
        }
        std::set<BufferedOutStream *, CompareStreamId> getMasterActionMessageTargets(ParticipantId answerTarget = 0, TransactionId answerId = 0,
            const std::set<ParticipantId> &targets = {}) const
        {
            return SyncableBase::getMasterActionMessageTargets(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableUnitBase>()), answerTarget, answerId, targets);
        }
        ParticipantId participantId()
        {
            return SyncableBase::participantId(parent());
        }

        void beginActionResponseMessage(BufferedOutStream *stream, TransactionId id) const
        {
            SyncableBase::beginActionResponseMessage(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableUnitBase>()), stream, id);
        }

        BufferedOutStream *beginActionResponseMessage(ParticipantId stream, TransactionId id) const
        {
            return SyncableBase::beginActionResponseMessage(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableUnitBase>()), stream, id);
        }

        bool isMaster() const
        {
            return SyncableBase::isMaster(parent());
        }

        const SerializableUnitBase *parent() const
        {
            return OffsetPtr::parent(this);
        }

		/*bool isActive() const
        {
                    return !parent() || parent()->isActive(parent()->type()->getIndex(OffsetPtr::template offset<SerializableUnitBase>()));
        }*/
    };
}
}
