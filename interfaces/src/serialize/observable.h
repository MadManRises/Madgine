#pragma once

namespace Engine {
		namespace Serialize {

			class INTERFACES_EXPORT Observable {
			public:
				virtual void readRequest(BufferedInOutStream &in) = 0;
				virtual void readAction(SerializeInStream &in) = 0;

			protected:
				Observable();		

				BufferedOutStream *getSlaveActionMessageTarget() const;
				std::list<BufferedOutStream*> getMasterActionMessageTargets(const std::list<ParticipantId> &targets = {}) const;
				void writeMasterActionMessageHeader(BufferedOutStream &out) const;

				ParticipantId id();
				bool isMaster() const;

				SerializableUnitBase *parent();
				
			private:		
				SerializableUnitBase *mUnit;
				size_t mIndex;

				
			};

		}
}