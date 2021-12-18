#pragma once



namespace Engine {
namespace Serialize {

    struct META_EXPORT SerializeManager {

		static constexpr ParticipantId sLocalMasterParticipantId = 1;

        SerializeManager(const std::string &name);
        SerializeManager(const SerializeManager &) = delete;
        SerializeManager(SerializeManager &&) noexcept;
        ~SerializeManager();

        const SyncableUnitMap &slavesMap() const;
        const SyncableUnitMap &mastersMap() const;

        void addSlaveMapping(UnitId id, SyncableUnitBase *item);
        void removeSlaveMapping(SyncableUnitBase *item);

        static UnitId generateMasterId(UnitId id, SyncableUnitBase *unit);
        static UnitId updateMasterId(UnitId id, SyncableUnitBase *unit);
        static void deleteMasterId(UnitId id, SyncableUnitBase *unit);

        bool isMaster(SerializeStreamData &stream) const;
        bool isMaster() const;

        static UnitId convertPtr(SerializeOutStream &out, const SyncableUnitBase *unit);
        static StreamResult convertPtr(SerializeInStream &in, UnitId unit, SyncableUnitBase *&out);

        const std::string &name() const;

		SerializeStreamData *getSlaveStreamData();        

    protected:
        void setSlaveStreamData(SerializeStreamData *data);

        static ParticipantId createStreamId();

		SyncableUnitBase *getByMasterId(UnitId unit);

    private:

        SyncableUnitMap mSlaveMappings;        

		SerializeStreamData *mSlaveStreamData = nullptr;

        std::string mName;
    };
}
}
