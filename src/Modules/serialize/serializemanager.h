#pragma once

#include "../signalslot/signal.h"

#include "serializableids.h"

#include "streams/serializestream.h"

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

        static size_t generateMasterId(size_t id, SerializableUnitBase *unit);
        static void deleteMasterId(size_t id, SerializableUnitBase *unit);

        bool isMaster(SerializeStreambuf *stream) const;
        bool isMaster() const;

        bool filter(const SerializableUnitBase *unit, ParticipantId id);
        void addFilter(std::function<bool(const SerializableUnitBase *, ParticipantId)>);

        static size_t convertPtr(const SerializeManager *mgr, SerializeOutStream &out, const SerializableUnitBase *unit);
        SerializableUnitBase *convertPtr(SerializeInStream &in, size_t unit);

        const std::string &name() const;

		SerializeStreambuf *getSlaveStreambuf();        

    protected:
        void setSlaveStreambuf(SerializeStreambuf *buf);

        static ParticipantId createStreamId();

		SerializableUnitBase *getByMasterId(size_t unit);

    private:

        SerializableUnitMap mSlaveMappings;        

		SerializeStreambuf *mSlaveStreambuf = nullptr;

        std::list<std::function<bool(const SerializableUnitBase *, ParticipantId)>> mFilters;

        std::string mName;
    };
}
}
