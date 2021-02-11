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

        bool isMaster(SerializeStreambuf *stream) const;
        bool isMaster() const;

        bool filter(const SerializableUnitBase *unit, ParticipantId id);
        void addFilter(std::function<bool(const SerializableUnitBase *, ParticipantId)>);

        static UnitId convertPtr(const SerializeManager *mgr, SerializeOutStream &out, const SyncableUnitBase *unit);
        SyncableUnitBase *convertPtr(SerializeInStream &in, UnitId unit);

        const std::string &name() const;

		SerializeStreambuf *getSlaveStreambuf();        

    protected:
        void setSlaveStreambuf(SerializeStreambuf *buf);

        static ParticipantId createStreamId();

		SyncableUnitBase *getByMasterId(UnitId unit);

    private:

        SyncableUnitMap mSlaveMappings;        

		SerializeStreambuf *mSlaveStreambuf = nullptr;

        std::list<std::function<bool(const SerializableUnitBase *, ParticipantId)>> mFilters;

        std::string mName;
    };
}
}
