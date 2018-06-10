#pragma once

#include "serializableunit.h"

namespace Engine
{
	namespace Serialize
	{
		class INTERFACES_EXPORT TopLevelSerializableUnitBase : public SerializableUnitBase
		{
		public:
			TopLevelSerializableUnitBase(size_t staticId = 0);
			TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase& other);
			TopLevelSerializableUnitBase(TopLevelSerializableUnitBase&& other) noexcept;
			virtual ~TopLevelSerializableUnitBase();

			void copyStaticSlaveId(const TopLevelSerializableUnitBase& other);

			BufferedOutStream* getSlaveMessageTarget() const;

			const std::list<SerializeManager*>& getMasterManagers() const;
			SerializeManager* getSlaveManager() const;

			bool addManager(SerializeManager* mgr);
			void removeManager(SerializeManager* mgr);

			bool updateManagerType(SerializeManager* mgr, bool isMaster);

			//bool isMaster() const;

			static ParticipantId getLocalMasterParticipantId();
			ParticipantId getSlaveParticipantId() const;

			void setStaticSlaveId(size_t staticId);
			void initSlaveId();

			size_t readId(SerializeInStream& in) override;

			const TopLevelSerializableUnitBase* topLevel() const override;

			std::set<BufferedOutStream*, CompareStreamId> getMasterMessageTargets() override;

		private:
			std::list<SerializeManager*> mMasterManagers;
			SerializeManager* mSlaveManager;
			size_t mStaticSlaveId;
		};

		template <class T>
		using TopLevelSerializableUnit = SerializableUnit<T, TopLevelSerializableUnitBase>;
	} // namespace Serialize
} // namespace Core