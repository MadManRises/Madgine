#pragma once

#include "serializableunit.h"

namespace Engine
{
	namespace Serialize
	{
		class MODULES_EXPORT TopLevelSerializableUnitBase : public SerializableUnitBase
		{
		public:
			TopLevelSerializableUnitBase(size_t staticId = 0);
			TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase& other);
			TopLevelSerializableUnitBase(TopLevelSerializableUnitBase&& other) noexcept;
			virtual ~TopLevelSerializableUnitBase();

			void copyStaticSlaveId(const TopLevelSerializableUnitBase& other);

			BufferedOutStream* getSlaveMessageTarget() const;

			const std::vector<SerializeManager*>& getMasterManagers() const;
			SerializeManager* getSlaveManager() const;

			bool addManager(SerializeManager* mgr);
			void removeManager(SerializeManager* mgr);

			bool updateManagerType(SerializeManager* mgr, bool isMaster);

			//bool isMaster() const;
			ParticipantId participantId() const;

			void setStaticSlaveId(size_t staticId);
			void initSlaveId();

			size_t readId(SerializeInStream& in) override;

			const TopLevelSerializableUnitBase* topLevel() const override;

			std::set<BufferedOutStream*, CompareStreamId> getMasterMessageTargets() override;

		private:
			std::vector<SerializeManager*> mMasterManagers;
			SerializeManager* mSlaveManager;
			size_t mStaticSlaveId;
		};

		template <class T>
		using TopLevelSerializableUnit = SerializableUnit<T, TopLevelSerializableUnitBase>;
	} // namespace Serialize
} // namespace Core
