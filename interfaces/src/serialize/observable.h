#pragma once

#include "streams/serializestream.h"

namespace Engine {
		namespace Serialize {

			class INTERFACES_EXPORT Observable {
			public:
				virtual void readRequest(BufferedInOutStream &in) = 0;
				virtual void readAction(SerializeInStream &in) = 0;

			protected:
				Observable();		

				BufferedOutStream *getSlaveActionMessageTarget() const;
				std::set<BufferedOutStream*, CompareStreamId> getMasterActionMessageTargets(const std::set<ParticipantId> &targets = {}) const;

				void beginActionResponseMessage(BufferedOutStream *stream);

				ParticipantId id();
				bool isMaster() const;

				SerializableUnitBase *parent();

			private:		
				SerializableUnitBase *mUnit;
				size_t mIndex;

				
			};

		}
}