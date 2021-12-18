#include "../metalib.h"

#include "serializemanager.h"

#include "syncableunit.h"

#include "streams/serializestream.h"

#include "serializableids.h"

namespace Engine {
namespace Serialize {

    static std::mutex sMasterMappingMutex;
    static SyncableUnitMap sMasterMappings;
    static UnitId sNextUnitId = RESERVED_ID_COUNT;
    static std::atomic<ParticipantId> sRunningStreamId = SerializeManager::sLocalMasterParticipantId;

    SerializeManager::SerializeManager(const std::string &name)
        : mName(name)
    {
    }

    SerializeManager::SerializeManager(SerializeManager &&other) noexcept
        : mSlaveMappings(std::move(other.mSlaveMappings))
        , mSlaveStreamData(std::exchange(other.mSlaveStreamData, nullptr))
        , mName(std::move(other.mName))
    {
        other.mSlaveMappings.clear();
    }

    SerializeManager::~SerializeManager() { }

    const SyncableUnitMap &SerializeManager::slavesMap() const
    {
        return mSlaveMappings;
    }

    const SyncableUnitMap &SerializeManager::mastersMap() const
    {
        return sMasterMappings;
    }

    void SerializeManager::addSlaveMapping(UnitId id, SyncableUnitBase *item)
    {
        assert(mSlaveMappings.find(id) == mSlaveMappings.end());
        mSlaveMappings[id] = item;
        // std::cout << "Slave: " << item->slaveId() << " -> add " <<
        // typeid(*item).name() << std::endl;
    }

    void SerializeManager::removeSlaveMapping(SyncableUnitBase *item)
    {
        size_t result = mSlaveMappings.erase(item->slaveId());
        assert(result == 1);
    }

    UnitId SerializeManager::generateMasterId(UnitId id,
        SyncableUnitBase *unit)
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

    UnitId SerializeManager::updateMasterId(UnitId id, SyncableUnitBase *unit)
    {
        if (id >= RESERVED_ID_COUNT) {
            std::lock_guard guard(sMasterMappingMutex);
            auto it = sMasterMappings.find(id);
            assert(it->second != unit);
            it->second = unit;
        } else {
            assert(id >= BEGIN_STATIC_ID_SPACE);
        }
        return id;
    }

    void SerializeManager::deleteMasterId(UnitId id, SyncableUnitBase *unit)
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

    bool SerializeManager::isMaster(SerializeStreamData &stream) const
    {
        return mSlaveStreamData != &stream;
    }

    bool SerializeManager::isMaster() const
    {
        return !mSlaveStreamData;
    }

    UnitId SerializeManager::convertPtr(SerializeOutStream &out,
        const SyncableUnitBase *unit)
    {
        return unit == nullptr
            ? NULL_UNIT_ID
            : out.isMaster() ? unit->masterId()
                             : unit->slaveId();
    }

    StreamResult SerializeManager::convertPtr(SerializeInStream &in,
        UnitId unit, SyncableUnitBase *&out)
    {
        if (unit == NULL_UNIT_ID) {
            out = nullptr;
            return {};
        }
        SyncableUnitBase *ptr = nullptr;

        if (in.isMaster()) {
            ptr = in.manager()->getByMasterId(unit);
        } else {
            auto it = sMasterMappings.find(unit);
            if (it != sMasterMappings.end())
                ptr = it->second;
        }
        if (!ptr) {
            return STREAM_INTEGRITY_ERROR(in, "Unknown Unit-Id (" << unit << ") used!");
        }
        out = ptr;
        return {};
    }

    SerializeStreamData *SerializeManager::getSlaveStreamData()
    {
        return mSlaveStreamData;
    }

    void SerializeManager::setSlaveStreamData(SerializeStreamData *data)
    {
        if (mSlaveStreamData && data)
            std::terminate();
        mSlaveStreamData = data;
    }

    ParticipantId SerializeManager::createStreamId() { return ++sRunningStreamId; }

    SyncableUnitBase *SerializeManager::getByMasterId(UnitId unit)
    {
        std::lock_guard guard(sMasterMappingMutex);
        auto it = sMasterMappings.find(unit);
        if (it == sMasterMappings.end())
            return nullptr;
        return it->second;
    }

    const std::string &SerializeManager::name() const
    {
        return mName;
    }

    //const std::string &SerializeManager::name() const { return mName; }
} // namespace Serialize
} // namespace Engine
