#pragma once

#include "syncableunit.h"

namespace Engine {
namespace Serialize {
    struct META_EXPORT TopLevelUnitBase : SyncableUnitBase {
        TopLevelUnitBase(UnitId staticId = 0);
        TopLevelUnitBase(const TopLevelUnitBase &other);
        TopLevelUnitBase(TopLevelUnitBase &&other) noexcept;
        ~TopLevelUnitBase();

        void sync();
        void unsync();

        BufferedOutStream &getSlaveMessageTarget() const;

        const std::vector<SyncManager *> &getManagers() const;
        SyncManager *getSlaveManager() const;

        bool addManager(SyncManager *mgr);
        void removeManager(SyncManager *mgr);

        bool updateManagerType(SyncManager *mgr, bool isMaster);

        ParticipantId participantId() const;

        void setStaticSlaveId(UnitId staticId);
        void initSlaveId(SerializeManager *mgr);

        std::set<BufferedOutStream *, CompareStreamId> getMasterMessageTargets() const;

    private:
        std::vector<SyncManager *> mManagers;
        SyncManager *mSlaveManager = nullptr;
        UnitId mStaticSlaveId;
    };

    template <typename T>
    using TopLevelUnit = SyncableUnit<T, TopLevelUnitBase>;
} // namespace Serialize
} // namespace Core
