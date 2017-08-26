#pragma once

namespace Engine {
	namespace Serialize {


		class INTERFACES_EXPORT Serializable {
		public:
			Serializable();
			Serializable(const Serializable &);
			Serializable(Serializable &&);

			Serializable &operator=(const Serializable &) = default;

			virtual void readState(SerializeInStream&) = 0;
			virtual void writeState(SerializeOutStream&) const = 0;

			virtual void applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map);
			virtual void writeCreationData(SerializeOutStream &out) const;
			virtual void activate();

			std::list<BufferedOutStream*> getMasterStateMessageTargets();

			void sendState();

			TopLevelSerializableUnitBase *topLevel() const;

		protected:
			SerializableUnitBase *unit() const;

		private:
			SerializableUnitBase *mUnit;

		};

	}
}