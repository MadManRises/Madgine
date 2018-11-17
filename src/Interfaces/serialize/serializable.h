#pragma once


namespace Engine
{
	namespace Serialize
	{
		class INTERFACES_EXPORT Serializable
		{
		public:
			explicit Serializable(bool local = false);
			Serializable(const Serializable&);
			Serializable(Serializable&&) noexcept;
			virtual ~Serializable();

			Serializable& operator=(const Serializable&);

			virtual void readState(SerializeInStream&) = 0;
			virtual void writeState(SerializeOutStream&) const = 0;

			virtual void applySerializableMap(const std::map<size_t, SerializableUnitBase *>& map);
			virtual void writeCreationData(SerializeOutStream& out) const;
			virtual void setDataSynced(bool b);
			virtual void setActive(bool active);


			std::set<BufferedOutStream*, CompareStreamId> getMasterStateMessageTargets() const;

			void sendState();

			const TopLevelSerializableUnitBase* topLevel() const;

			SerializableUnitBase* unit() const;

		protected:
			bool isActive() const;
			bool isSynced() const;

		private:
			SerializableUnitBase* mUnit;
			bool mActive = false;
		};
	}
}
