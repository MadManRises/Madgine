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

    template <typename PtrOffset>
    struct Syncable : SyncableBase {
    protected:
        BufferedOutStream *getSlaveActionMessageTarget() const
        {
            return SyncableBase::getSlaveActionMessageTarget(parent(), parent()->type()->getIndex(PtrOffset::template offset<SerializableUnitBase, SyncableBase>()));
        }
        std::set<BufferedOutStream *, CompareStreamId> getMasterActionMessageTargets(
            const std::set<ParticipantId> &targets = {}) const
        {
            return SyncableBase::getMasterActionMessageTargets(parent(), parent()->type()->getIndex(PtrOffset::template offset<SerializableUnitBase, SyncableBase>()), targets);
        }
        ParticipantId participantId()
        {
            return SyncableBase::participantId(parent());
        }

        void beginActionResponseMessage(BufferedOutStream *stream) const
        {
            SyncableBase::beginActionResponseMessage(parent(), parent()->type()->getIndex(PtrOffset::template offset<SerializableUnitBase, SyncableBase>()), stream);
        }

        bool isMaster() const
        {
            return SyncableBase::isMaster(parent());
        }

        const SerializableUnitBase *parent() const
        {
            return PtrOffset::parent(this);
        }
    };
}
}
