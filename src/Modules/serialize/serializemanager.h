#pragma once

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT SerializeManager {

		static constexpr ParticipantId sLocalMasterParticipantId = 1;

        SerializeManager(const std::string &name);
        SerializeManager(const SerializeManager &) = delete;
        SerializeManager(SerializeManager &&) noexcept;
        ~SerializeManager();

        const SerializableUnitMap &slavesMap() const;
        const SerializableUnitMap &mastersMap() const;

        void addSlaveMapping(SerializableUnitBase *item);
        void removeSlaveMapping(SerializableUnitBase *item);

        static UnitId generateMasterId(UnitId id, SerializableUnitBase *unit);
        static UnitId updateMasterId(UnitId id, SerializableUnitBase *unit);
        static void deleteMasterId(UnitId id, SerializableUnitBase *unit);

        bool isMaster(SerializeStreambuf *stream) const;
        bool isMaster() const;

        bool filter(const SerializableUnitBase *unit, ParticipantId id);
        void addFilter(std::function<bool(const SerializableUnitBase *, ParticipantId)>);

        static UnitId convertPtr(const SerializeManager *mgr, SerializeOutStream &out, const SerializableUnitBase *unit);
        SerializableUnitBase *convertPtr(SerializeInStream &in, UnitId unit);

        const std::string &name() const;

		SerializeStreambuf *getSlaveStreambuf();        

    protected:
        void setSlaveStreambuf(SerializeStreambuf *buf);

        static ParticipantId createStreamId();

		SerializableUnitBase *getByMasterId(UnitId unit);

    private:

        SerializableUnitMap mSlaveMappings;        

		SerializeStreambuf *mSlaveStreambuf = nullptr;

        std::list<std::function<bool(const SerializableUnitBase *, ParticipantId)>> mFilters;

        std::string mName;
    };
}
}
