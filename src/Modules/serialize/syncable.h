#pragma once

#include "streams/comparestreamid.h"

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT SyncableBase {
        ~SyncableBase() = default;

        /*virtual void readRequest(BufferedInOutStream &in) = 0;
        virtual void readAction(SerializeInStream &in) = 0;*/

    protected:
        BufferedOutStream *getSlaveActionMessageTarget(const SerializableUnitBase *parent, size_t index) const;
        std::set<BufferedOutStream *, CompareStreamId> getMasterActionMessageTargets(const SerializableUnitBase *parent, size_t index,
            const std::set<ParticipantId> &targets = {}) const;
        ParticipantId participantId(const SerializableUnitBase *parent);

        void beginActionResponseMessage(const SerializableUnitBase *parent, size_t index, BufferedOutStream *stream) const;

        bool isMaster(const SerializableUnitBase *parent) const;
    };

    template <typename OffsetPtr>
    struct Syncable : SyncableBase {
    protected:
        BufferedOutStream *getSlaveActionMessageTarget() const
        {
            return SyncableBase::getSlaveActionMessageTarget(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableUnitBase>()));
        }
        std::set<BufferedOutStream *, CompareStreamId> getMasterActionMessageTargets(
            const std::set<ParticipantId> &targets = {}) const
        {
            return SyncableBase::getMasterActionMessageTargets(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableUnitBase>()), targets);
        }
        ParticipantId participantId()
        {
            return SyncableBase::participantId(parent());
        }

        void beginActionResponseMessage(BufferedOutStream *stream) const
        {
            SyncableBase::beginActionResponseMessage(parent(), parent()->serializeType()->getIndex(OffsetPtr::template offset<SerializableUnitBase>()), stream);
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
