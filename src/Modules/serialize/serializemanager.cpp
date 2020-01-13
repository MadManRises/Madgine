#include "../moduleslib.h"

#include "serializemanager.h"

#include "streams/bufferedstream.h"

#include "serializeexception.h"

#include "toplevelserializableunit.h"

#include "../serialize/streams/buffered_streambuf.h"

#include "../generic/transformIt.h"

namespace Engine {
namespace Serialize {

    static std::mutex sMasterMappingMutex;
    static SerializableUnitMap sMasterMappings;
    static size_t sNextUnitId = RESERVED_ID_COUNT;
    static std::atomic<ParticipantId> sRunningStreamId = SerializeManager::sLocalMasterParticipantId;

    SerializeManager::SerializeManager(const std::string &name)
        : mName(name)
    {
    }

    SerializeManager::SerializeManager(SerializeManager &&other) noexcept
        : mSlaveMappings(std::move(other.mSlaveMappings))
        , mSlaveStreambuf(std::exchange(other.mSlaveStreambuf, nullptr))
        , mFilters(std::move(other.mFilters))
        , mName(std::move(other.mName))
    {
        other.mSlaveMappings.clear();
    }

    SerializeManager::~SerializeManager() {}

    const SerializableUnitMap &SerializeManager::slavesMap() const
    {
        return mSlaveMappings;
    }

	const SerializableUnitMap &SerializeManager::mastersMap() const
    {
        return sMasterMappings;
    }

    void SerializeManager::addSlaveMapping(SerializableUnitBase *item)
    {
        assert(mSlaveMappings.find(item->slaveId()) == mSlaveMappings.end());
        mSlaveMappings[item->slaveId()] = item;
        // std::cout << "Slave: " << item->slaveId() << " -> add " <<
        // typeid(*item).name() << std::endl;
    }

    void SerializeManager::removeSlaveMapping(SerializableUnitBase *item)
    {
        size_t result = mSlaveMappings.erase(item->slaveId());
        assert(result == 1);
    }

    size_t SerializeManager::generateMasterId(size_t id,
        SerializableUnitBase *unit)
    {
        if (id == 0) {
            std::lock_guard guard(sMasterMappingMutex);
            id = ++sNextUnitId;
            sMasterMappings[id] = unit;
        } else {
            assert(id >= BEGIN_STATIC_ID_SPACE);
            if (id >= RESERVED_ID_COUNT) {
                bool b = sMasterMappings.try_emplace(id, unit).second;
                assert(b);
            }
        }
        return id;
    }

    void SerializeManager::deleteMasterId(size_t id, SerializableUnitBase *unit)
    {
        if (id >= RESERVED_ID_COUNT) {
            std::lock_guard guard(sMasterMappingMutex);
            auto it = sMasterMappings.find(id);
            assert(it->second == unit);
            sMasterMappings.erase(it);
        } else {
            assert(id >= BEGIN_STATIC_ID_SPACE);
        }
    }

    bool SerializeManager::isMaster(SerializeStreambuf *stream) const
    {
        return !mSlaveStreambuf || mSlaveStreambuf != stream;
    }

    bool SerializeManager::isMaster() const
    {
        return !mSlaveStreambuf;
    }

    bool SerializeManager::filter(const SerializableUnitBase *unit,
        ParticipantId id)
    {
        for (auto &f : mFilters) {
            if (!f(unit, id)) {
                return false;
            }
        }
        return true;
    }

    void SerializeManager::addFilter(
        std::function<bool(const SerializableUnitBase *, ParticipantId)> f)
    {
        mFilters.push_back(f);
    }

    size_t SerializeManager::convertPtr(const SerializeManager *mgr, SerializeOutStream &out,
        const SerializableUnitBase *unit)
    {
        return unit == nullptr
            ? NULL_UNIT_ID
            : (!mgr || mgr->isMaster(&out.buffer())) ? unit->masterId()
                                                     : unit->slaveId();
    }

    SerializableUnitBase *SerializeManager::convertPtr(SerializeInStream &in,
        size_t unit)
    {
        if (unit == NULL_UNIT_ID)
            return nullptr;
        try {
            if (mSlaveStreambuf && (&in.buffer() == mSlaveStreambuf)) {
                return mSlaveMappings.at(unit);
            } else {
                return getByMasterId(unit);
            }
        } catch (const std::out_of_range &) {
            std::stringstream ss;
            ss << "Unknown Unit-Id (" << unit
               << ") used! Possible binary mismatch!";
            throw SerializeException(ss.str());
        }
    }

    SerializeStreambuf *SerializeManager::getSlaveStreambuf()
    {
        return mSlaveStreambuf;
    }

    void SerializeManager::setSlaveStreambuf(SerializeStreambuf *buf)
    {
        if (mSlaveStreambuf && buf)
            std::terminate();
        mSlaveStreambuf = buf;
    }

    ParticipantId SerializeManager::createStreamId() { return ++sRunningStreamId; }

    SerializableUnitBase *SerializeManager::getByMasterId(size_t unit)
    {
        std::lock_guard guard(sMasterMappingMutex);
        return sMasterMappings.at(unit);
    }

    const std::string &SerializeManager::name() const
    {
        return mName;
    }

    //const std::string &SerializeManager::name() const { return mName; }
} // namespace Serialize
} // namespace Engine
