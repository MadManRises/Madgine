#pragma once

#include "../streams/comparestreamid.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT SyncableBase {
        ~SyncableBase() = default;

        /*virtual void readRequest(BufferedInOutStream &in) = 0;
        virtual void readAction(SerializeInStream &in) = 0;*/

    protected:
        FormattedBufferedStream &getSlaveRequestMessageTarget(const SyncableUnitBase *parent, OffsetPtr offset, ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback) const;
        FormattedBufferedStream &getSlaveRequestMessageTarget(const SyncableUnitBase *parent, uint8_t index, ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback) const;
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterActionMessageTargets(const SyncableUnitBase *parent, OffsetPtr offset, ParticipantId answerTarget, TransactionId answerId,
            const std::set<ParticipantId> &targets = {}) const;
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterActionMessageTargets(const SyncableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId,
            const std::set<ParticipantId> &targets = {}) const;
        FormattedBufferedStream &getMasterRequestResponseTarget(const SyncableUnitBase *parent, OffsetPtr offset, ParticipantId answerTarget, TransactionId answerId) const;
        FormattedBufferedStream &getMasterRequestResponseTarget(const SyncableUnitBase *parent, uint8_t index, ParticipantId answerTarget, TransactionId answerId) const;
        ParticipantId participantId(const SerializableUnitBase *parent);

        void writeAction(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId answerTarget, TransactionId answerId, const std::set<ParticipantId> &targets = {}) const;
        void writeRequest(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback) const;
        void writeRequestResponse(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId answerTarget, TransactionId answerId) const;

        void beginRequestResponseMessage(const SyncableUnitBase *parent, OffsetPtr offset, FormattedBufferedStream &stream, TransactionId id) const;
        void beginRequestResponseMessage(const SyncableUnitBase *parent, uint8_t index, FormattedBufferedStream &stream, TransactionId id) const;

        bool isMaster(const SyncableUnitBase *parent) const;
    };

    template <typename OffsetPtr>
    struct Syncable : SyncableBase {

        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterActionMessageTargets(ParticipantId answerTarget = 0, TransactionId answerId = 0,
            const std::set<ParticipantId> &targets = {}) const
        {
            return SyncableBase::getMasterActionMessageTargets(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), answerTarget, answerId, targets);
        }

        FormattedBufferedStream &getSlaveRequestMessageTarget(ParticipantId requester = 0, TransactionId requesterTransactionId = 0, std::function<void(void *)> callback = {}) const
        {
            return SyncableBase::getSlaveRequestMessageTarget(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableDataUnit>()), requester, requesterTransactionId, std::move(callback));
        }

        ParticipantId participantId()
        {
            return SyncableBase::participantId(parent());
        }

        void writeAction(const void *data, ParticipantId answerTarget = 0, TransactionId answerId = 0, const std::set<ParticipantId> &targets = {}) const
        {
            SyncableBase::writeAction(parent(), OffsetPtr::template offset<SerializableDataUnit>(), data, answerTarget, answerId, targets);
        }

        void writeRequest(const void *data, ParticipantId requester = 0, TransactionId requesterTransactionId = 0, Lambda<void(void *)> callback = {}) const
        {
            SyncableBase::writeRequest(parent(), OffsetPtr::template offset<SerializableDataUnit>(), data, requester, requesterTransactionId, std::move(callback));
        }

        void writeRequestResponse(const void *data, ParticipantId answerTarget, TransactionId answerId) const
        {
            SyncableBase::writeRequestResponse(parent(),OffsetPtr::template offset<SerializableDataUnit>(), data, answerTarget, answerId);
        }

        void beginRequestResponseMessage(FormattedBufferedStream &stream, TransactionId id) const
        {
            SyncableBase::beginRequestResponseMessage(parent(), OffsetPtr::template offset<SerializableDataUnit>(), stream, id);
        }

        FormattedBufferedStream &getRequestResponseTarget(ParticipantId stream, TransactionId id) const
        {            
            return SyncableBase::getMasterRequestResponseTarget(parent(), OffsetPtr::template offset<SerializableDataUnit>(), stream, id);
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
