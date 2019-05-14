#pragma once

#include "streams/comparestreamid.h"

namespace Engine
{
	namespace Serialize
	{
		
		class INTERFACES_EXPORT Observable
		{
		public:
			virtual ~Observable() = default;

			virtual void readRequest(BufferedInOutStream& in) = 0;
			virtual void readAction(SerializeInStream& in) = 0;

		protected:
			Observable();
			Observable(const Observable &);
			Observable(Observable &&) noexcept;

			BufferedOutStream* getSlaveActionMessageTarget() const;
			std::set<BufferedOutStream*, CompareStreamId> getMasterActionMessageTargets(
				const std::set<ParticipantId>& targets = {}) const;
            ParticipantId participantId();

			void beginActionResponseMessage(BufferedOutStream* stream) const;

			bool isMaster() const;

			SerializableUnitBase* parent() const;

		private:
			SerializableUnitBase* mUnit;
			size_t mIndex;
		};
	}
}
