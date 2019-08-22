#pragma once

#include "streams/comparestreamid.h"

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT ObservableBase {
        virtual ~ObservableBase() = default;

        virtual void readRequest(BufferedInOutStream &in) = 0;
        virtual void readAction(SerializeInStream &in) = 0;

		protected:
        BufferedOutStream *getSlaveActionMessageTarget(const SerializableUnitBase *parent, size_t offset) const;
                    std::set<BufferedOutStream *, CompareStreamId> getMasterActionMessageTargets(const SerializableUnitBase *parent, size_t offset,
            const std::set<ParticipantId> &targets = {}) const;
        ParticipantId participantId(const SerializableUnitBase *parent);

        void beginActionResponseMessage(const SerializableUnitBase *parent, size_t offset, BufferedOutStream *stream) const;

        bool isMaster(const SerializableUnitBase *parent) const;
    };

    template <typename PtrOffset>
    struct Observable : ObservableBase {
    protected:
		BufferedOutStream* getSlaveActionMessageTarget() const {
                    return ObservableBase::getSlaveActionMessageTarget(parent(), PtrOffset::template offset<SerializableUnitBase, ObservableBase>());
		}
		std::set<BufferedOutStream*, CompareStreamId> getMasterActionMessageTargets(
			const std::set<ParticipantId>& targets = {}) const {
                    return ObservableBase::getMasterActionMessageTargets(parent(), PtrOffset::template offset<SerializableUnitBase, ObservableBase>(), targets);
		}
		ParticipantId participantId() {
                    return ObservableBase::participantId(parent());
				}

		void beginActionResponseMessage(BufferedOutStream* stream) const {
                    ObservableBase::beginActionResponseMessage(parent(), PtrOffset::template offset<SerializableUnitBase, ObservableBase>(), stream);
		}

		bool isMaster() const {
                    return ObservableBase::isMaster(parent());
		}

        const SerializableUnitBase *parent() const
        {
            return PtrOffset::parent(this);
        }
    };
}
}
