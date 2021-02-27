#pragma once

#include "streams/comparestreamid.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT SyncableBase {
        ~SyncableBase() = default;

        /*virtual void readRequest(BufferedInOutStream &in) = 0;
        virtual void readAction(SerializeInStream &in) = 0;*/

    protected:
        BufferedOutStream &getSlaveActionMessageTarget(const SyncableUnitBase *parent, uint8_t index, ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback) const;
        std::set<BufferedOutStream *, CompareStreamId> getMasterActionMessageTargets(const SyncableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId,
            const std::set<ParticipantId> &targets = {}) const;
        BufferedOutStream &getMasterActionResponseTarget(const SyncableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId) const;
        ParticipantId participantId(const SerializableUnitBase *parent);

        void writeAction(const SyncableUnitBase *parent, uint8_t index, const void *data, ParticipantId answerTarget, TransactionId answerId, const std::set<ParticipantId> &targets = {}) const;
        void writeRequest(const SyncableUnitBase *parent, uint8_t index, const void *data, ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback) const;
        void writeActionResponse(const SyncableUnitBase *parent, uint8_t index, const void *data, ParticipantId answerTarget, TransactionId answerId) const;

        void beginActionResponseMessage(const SyncableUnitBase *parent, uint8_t index, BufferedOutStream &stream, TransactionId id) const;

        bool isMaster(const SyncableUnitBase *parent) const;
    };

    template <typename OffsetPtr>
    struct Syncable : SyncableBase {

        std::set<BufferedOutStream *, CompareStreamId> getMasterActionMessageTargets(ParticipantId answerTarget = 0, TransactionId answerId = 0,
            const std::set<ParticipantId> &targets = {}) const
        {
            return SyncableBase::getMasterActionMessageTargets(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), answerTarget, answerId, targets);
        }

        BufferedOutStream &getSlaveActionMessageTarget(ParticipantId requester = 0, TransactionId requesterTransactionId = 0, std::function<void(void *)> callback = {}) const
        {
            return SyncableBase::getSlaveActionMessageTarget(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), requester, requesterTransactionId, std::move(callback));
        }

        ParticipantId participantId()
        {
            return SyncableBase::participantId(parent());
        }

        void writeAction(const void *data, ParticipantId answerTarget = 0, TransactionId answerId = 0, const std::set<ParticipantId> &targets = {}) const
        {
            SyncableBase::writeAction(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), data, answerTarget, answerId, targets);
        }

        void writeRequest(const void *data, ParticipantId requester = 0, TransactionId requesterTransactionId = 0, std::function<void(void *)> callback = {}) const
        {
            SyncableBase::writeRequest(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), data, requester, requesterTransactionId, std::move(callback));
        }

        void writeActionResponse(const void *data, ParticipantId answerTarget, TransactionId answerId) const
        {
            SyncableBase::writeActionResponse(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), data, answerTarget, answerId);
        }

        void beginActionResponseMessage(BufferedOutStream &stream, TransactionId id) const
        {
            SyncableBase::beginActionResponseMessage(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), stream, id);
        }

        BufferedOutStream &getActionResponseTarget(ParticipantId stream, TransactionId id) const
        {            
            return SyncableBase::getMasterActionResponseTarget(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), stream, id);
        }

        bool isMaster() const
        {
            return SyncableBase::isMaster(parent());
        }

        const SyncableUnitBase *parent() const
        {
            return OffsetPtr::parent(this);
        }

        /*bool isActive() const
        {
                    return !parent() || parent()->isActive(parent()->type()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()));
        }*/
    };
}
}
