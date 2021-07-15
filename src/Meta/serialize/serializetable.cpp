#include "../metalib.h"

#include "serializetable.h"

#include "formatter.h"

#include "streams/bufferedstream.h"

#include "serializableunit.h"

#include "serializer.h"

#include "streams/operations.h"

namespace Engine {
namespace Serialize {

    void SerializeTable::writeState(const SerializableDataUnit *unit, SerializeOutStream &out, CallerHierarchyBasePtr hierarchy) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mWriteState(unit, out, it->second.mFieldName, hierarchy);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    StreamResult SerializeTable::readState(SerializableDataUnit *unit, SerializeInStream &in, StateTransmissionFlags flags, CallerHierarchyBasePtr hierarchy) const
    {
        Formatter &format = in.format();

        StreamResult result = [&]() -> StreamResult {
            if (format.mSupportNameLookup) {
                std::string name = format.lookupFieldName(in);
                while (!name.empty()) {
                    bool found = false;
                    const SerializeTable *table = this;
                    while (table && !found) {
                        for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                            if (name == it->second.mFieldName) {
                                STREAM_PROPAGATE_ERROR(it->second.mReadState(unit, in, it->second.mFieldName, hierarchy));
                                found = true;
                                break;
                            }
                        }
                        table = table->mBaseType ? &table->mBaseType() : nullptr;
                    }
                    if (!found)
                        return STREAM_PARSE_ERROR(in, "Could not find field '" << name << "'");
                    name = format.lookupFieldName(in);
                }
            } else {
                const SerializeTable *table = this;
                while (table) {
                    for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                        STREAM_PROPAGATE_ERROR(it->second.mReadState(unit, in, it->second.mFieldName, hierarchy));
                    }
                    table = table->mBaseType ? &table->mBaseType() : nullptr;
                }
            }
            return {};
        }();

        if (flags & StateTransmissionFlags_ApplyMap) {
            assert(in.manager());
            applySerializableMap(unit, in, result.mState == StreamState::OK);
        }
        return result;
    }

    StreamResult SerializeTable::readState(SerializableUnitBase *unit, SerializeInStream &in, StateTransmissionFlags flags, CallerHierarchyBasePtr hierarchy) const
    {
        bool wasActive = unit->mActiveIndex > 0; //Not exact, but active unit with 0 members doesn't matter
        if (wasActive) {
            setActive(unit, false, false);
        }

        StreamResult result = readState(static_cast<SerializableDataUnit *>(unit), in, flags, hierarchy);
        
        if (wasActive) {
            setActive(unit, true, false);
        }

        return result;
    }

    StreamResult SerializeTable::readAction(SerializableUnitBase *unit, SerializeInStream &in, PendingRequest *request) const
    {
        uint8_t index;
        in >> index;
        return get(index).mReadAction(unit, in, request);
    }

    StreamResult SerializeTable::readRequest(SerializableUnitBase *unit, BufferedInOutStream &inout, TransactionId id) const
    {
        uint8_t index;
        inout >> index;
        return get(index).mReadRequest(unit, inout, id);
    }

    void SerializeTable::applySerializableMap(SerializableDataUnit *unit, SerializeInStream &in, bool success) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mApplySerializableMap(unit, in, success);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::setDataSynced(SerializableUnitBase *unit, bool b) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mSetDataSynced(unit, b);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::setActive(SerializableUnitBase *unit, bool active, bool existenceChanged) const
    {
        //TODO: call onActivate for ALL base classes
        if (active)
            assert(unit->mActiveIndex == 0);
        else if (mCallbacks.onActivate)
            mCallbacks.onActivate(unit, active, existenceChanged);

        //TODO: Start with base
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                if (active)
                    ++unit->mActiveIndex;
                else {
                    assert(unit->mActiveIndex > 0);
                    --unit->mActiveIndex;
                }
                it->second.mSetActive(unit, active, existenceChanged);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
        if (!active)
            assert(unit->mActiveIndex == 0);
        else if (mCallbacks.onActivate)
            mCallbacks.onActivate(unit, active, existenceChanged);
    }

    void SerializeTable::setParent(SerializableUnitBase *unit) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mSetParent(unit);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::writeAction(const SerializableUnitBase *parent, uint8_t index, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *data) const
    {
        get(index).mWriteAction(parent, outStreams, data);
    }

    void SerializeTable::writeRequest(const SerializableUnitBase *parent, uint8_t index, BufferedOutStream &out, const void *data) const
    {
        get(index).mWriteRequest(parent, out, data);
    }

    uint8_t SerializeTable::getIndex(OffsetPtr offset) const
    {
        uint8_t index = 0;

        std::stack<const SerializeTable *> tables;
        const SerializeTable *table = this;
        while (table) {
            tables.push(table);
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }

        while (!tables.empty()) {
            table = tables.top();
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                if (it->second.mOffset() == offset) {
                    return index;
                }
                ++index;
            }
            tables.pop();
        }

        assert(false);
        throw 0;
    }

    const Serializer &SerializeTable::get(uint8_t index) const
    {
        std::stack<const SerializeTable *> tables;
        const SerializeTable *table = this;
        while (table) {
            tables.push(table);
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }

        while (!tables.empty()) {
            table = tables.top();
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                if (index == 0) {
                    return it->second;
                }
                --index;
            }
            tables.pop();
        }

        //Corrupt package
        std::terminate();
    }

}
}