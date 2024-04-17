#pragma once

#include "../streams/comparestreamid.h"

#include "../streams/pendingrequest.h"

namespace Engine {
namespace Serialize {

    META_EXPORT FormattedBufferedStream &getSlaveRequestMessageTarget(const SyncableUnitBase *unit, ParticipantId requester, MessageId requesterTransactionId, GenericMessageReceiver receiver);
    META_EXPORT std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterActionMessageTargets(const SyncableUnitBase *unit, ParticipantId answerTarget, MessageId answerId,
        const std::set<ParticipantId> &targets = {});
    void beginRequestResponseMessage(const SyncableUnitBase *unit, FormattedBufferedStream &stream, MessageId id);


    struct META_EXPORT SyncableBase {
    };

    template <typename OffsetPtr>
    struct Syncable : SyncableBase {

        std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> getMasterActionMessageTargets(ParticipantId answerTarget = 0, MessageId answerId = 0,
            const std::set<ParticipantId> &targets = {}) const
        {
            return getMasterActionMessageTargets(parent(), answerTarget, answerId, targets);
        }

        FormattedBufferedStream &getSlaveRequestMessageTarget(ParticipantId requester = 0, MessageId requesterTransactionId = 0, GenericMessageReceiver receiver = {}) const
        {
            return Serialize::getSlaveRequestMessageTarget(parent(), requester, requesterTransactionId, std::move(receiver));
        }

        ParticipantId participantId()
        {
            return parent()->participantId();
        }

        template <typename... Args>
        void writeAction(ParticipantId answerTarget, MessageId answerId, Args &&...args) const
        {
            parent()->writeAction(static_cast<const typename OffsetPtr::member_type *>(this), answerTarget, answerId, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void writeRequest(ParticipantId requester, MessageId requesterTransactionId, Args &&...args) const
        {
            parent()->writeRequest(static_cast<const typename OffsetPtr::member_type *>(this), requester, requesterTransactionId, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void writeRequest(GenericMessageReceiver receiver, Args &&...args) const
        {
            parent()->writeRequest(static_cast<const typename OffsetPtr::member_type *>(this), std::move(receiver), std::forward<Args>(args)...);
        }

        template <typename... Args>
        void writeRequestResponse(ParticipantId answerTarget, MessageId answerId, Args &&...args) const
        {
            parent()->writeRequestResponse(static_cast<const typename OffsetPtr::member_type *>(this), answerTarget, answerId, std::forward<Args>(args)...);
        }

        void beginRequestResponseMessage(FormattedBufferedStream &stream, MessageId id) const
        {
            Serialize::beginRequestResponseMessage(parent(), stream, id);
        }

        FormattedBufferedStream &getRequestResponseTarget(ParticipantId stream, MessageId id) const
        {
            return getMasterRequestResponseTarget(parent(), stream, id);
        }

        bool isMaster() const
        {
            return parent()->isMaster();
        }

        auto parent() const
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
