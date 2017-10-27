#pragma once

#include "streams/serializestream.h"

namespace Engine {
	namespace Serialize {


		class INTERFACES_EXPORT Serializable {
		public:
			Serializable();
			Serializable(const Serializable &);
			Serializable(Serializable &&);
			virtual ~Serializable();

			Serializable &operator=(const Serializable &);

			virtual void readState(SerializeInStream&) = 0;
			virtual void writeState(SerializeOutStream&) const = 0;

			virtual void applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map);
			virtual void writeCreationData(SerializeOutStream &out) const;
			virtual void setActiveFlag(bool b);
			virtual void notifySetActive(bool active);


			std::set<BufferedOutStream*, CompareStreamId> getMasterStateMessageTargets();

			void sendState();

			const TopLevelSerializableUnitBase *topLevel() const;

			SerializableUnitBase *unit() const;

		protected:
			bool isLocallyActive();
			bool isActive();

		private:
			SerializableUnitBase *mUnit;
			bool mLocallyActive;

		};

	}
}