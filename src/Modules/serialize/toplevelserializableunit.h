#pragma once

#include "serializableunit.h"

namespace Engine {
namespace Serialize {
    class MODULES_EXPORT TopLevelSerializableUnitBase : public SerializableUnitBase {
    public:
        TopLevelSerializableUnitBase(size_t staticId = 0);
        TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase &other);
        TopLevelSerializableUnitBase(TopLevelSerializableUnitBase &&other) noexcept;
        ~TopLevelSerializableUnitBase();

        BufferedOutStream *getSlaveMessageTarget() const;

        const std::vector<SyncManager *> &getMasterManagers() const;
        SyncManager *getSlaveManager() const;

        bool addManager(SyncManager *mgr);
        void removeManager(SyncManager *mgr);

        bool updateManagerType(SyncManager *mgr, bool isMaster);

        ParticipantId participantId() const;

        void setStaticSlaveId(size_t staticId);
        void initSlaveId(SerializeManager *mgr);

        std::set<BufferedOutStream *, CompareStreamId> getMasterMessageTargets() const;

    private:
        std::vector<SyncManager *> mMasterManagers;
        SyncManager *mSlaveManager = nullptr;
        size_t mStaticSlaveId;
    };

    template <class T>
    using TopLevelSerializableUnit = SerializableUnit<T, TopLevelSerializableUnitBase>;
} // namespace Serialize
} // namespace Core
