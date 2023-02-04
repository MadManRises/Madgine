#pragma once

#include "../streams/comparestreamid.h"

#include "../streams/pendingrequest.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT SyncableBase {
        ~SyncableBase() = default;

        /*virtual void readRequest(BufferedInOutStream &in) = 0;
        virtual void readAction(SerializeInStream &in) = 0;*/

    protected:
        FormattedBufferedStream &getSlaveRequestMessageTarget(const SyncableUnitBase *parent, ParticipantId requester, MessageId requesterTransactionId, GenericMessageReceiver receiver) const;
        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterActionMessageTargets(const SyncableUnitBase *parent, ParticipantId answerTarget, MessageId answerId,
            const std::set<ParticipantId> &targets = {}) const;
        FormattedBufferedStream &getMasterRequestResponseTarget(const SyncableUnitBase *parent, ParticipantId answerTarget, MessageId answerId) const;
        ParticipantId participantId(const SerializableUnitBase *parent);

        void writeAction(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId answerTarget, MessageId answerId, const std::set<ParticipantId> &targets = {}) const;
        void writeRequest(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId requester = 0, MessageId requesterTransactionId = 0, GenericMessageReceiver receiver = {}) const;
        void writeRequestResponse(const SyncableUnitBase *parent, OffsetPtr offset, const void *data, ParticipantId answerTarget, MessageId answerId) const;

        void beginRequestResponseMessage(const SyncableUnitBase *parent, FormattedBufferedStream &stream, MessageId id) const;
        
        bool isMaster(const SyncableUnitBase *parent) const;
    };

    template <typename OffsetPtr>
    struct Syncable : SyncableBase {

        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterActionMessageTargets(ParticipantId answerTarget = 0, MessageId answerId = 0,
            const std::set<ParticipantId> &targets = {}) const
        {
            return SyncableBase::getMasterActionMessageTargets(parent(), answerTarget, answerId, targets);
        }

        FormattedBufferedStream &getSlaveRequestMessageTarget(ParticipantId requester = 0, MessageId requesterTransactionId = 0, GenericMessageReceiver receiver = {}) const
        {
            return SyncableBase::getSlaveRequestMessageTarget(parent(), requester, requesterTransactionId, std::move(receiver));
        }

        ParticipantId participantId()
        {
            return SyncableBase::participantId(parent());
        }

        template <typename... Args>
        void writeAction(ParticipantId answerTarget, MessageId answerId, Args&&... args) const
        {
            std::tuple<std::decay_t<Args>...> data { std::forward<Args>(args)... };
            SyncableBase::writeAction(parent(), OffsetPtr::template offset<SerializableDataUnit>(), &data, answerTarget, answerId, {});
        }
        
        /* template <typename... Args>
        void writeRequest(Args&&... args) const
        {
            std::tuple<std::decay_t<Args>...> data { std::forward<Args>(args)... };
            SyncableBase::writeRequest(parent(), OffsetPtr::template offset<SerializableDataUnit>(), &data);
        }*/

        template <typename... Args>
        void writeRequest(ParticipantId requester, MessageId requesterTransactionId, Args&&... args) const
        {
            std::tuple<std::decay_t<Args>...> data { std::forward<Args>(args)... };
            SyncableBase::writeRequest(parent(), OffsetPtr::template offset<SerializableDataUnit>(), &data, requester, requesterTransactionId);
        }
           
        template <typename... Args>
        void writeRequest(GenericMessageReceiver receiver, Args &&...args) const
        {
            std::tuple<std::decay_t<Args>...> data { std::forward<Args>(args)... };
            SyncableBase::writeRequest(parent(), OffsetPtr::template offset<SerializableDataUnit>(), &data, 0, 0, std::move(receiver));
        }

        template <typename... Args>
        void writeRequestResponse(ParticipantId answerTarget, MessageId answerId, Args &&...args) const
        {
            std::tuple<std::decay_t<Args>...> data { std::forward<Args>(args)... };
            SyncableBase::writeRequestResponse(parent(), OffsetPtr::template offset<SerializableDataUnit>(), &data, answerTarget, answerId);
        }

        void beginRequestResponseMessage(FormattedBufferedStream &stream, MessageId id) const
        {
            SyncableBase::beginRequestResponseMessage(parent(), stream, id);
        }

        FormattedBufferedStream &getRequestResponseTarget(ParticipantId stream, MessageId id) const
        {
            return SyncableBase::getMasterRequestResponseTarget(parent(), stream, id);
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
