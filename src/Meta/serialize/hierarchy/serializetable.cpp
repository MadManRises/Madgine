#include "../../metalib.h"

#include "serializer.h"
#include "serializetable.h"

#include "statetransmissionflags.h"

#include "serializableunit.h"

#include "../operations.h"

#include "../streams/formattedbufferedstream.h"

#include "Generic/offsetptr.h"

#include "../streams/comparestreamid.h"

namespace Engine {
namespace Serialize {

    void SerializeTable::writeState(const SerializableDataUnit *unit, FormattedSerializeStream &out, CallerHierarchyBasePtr hierarchy) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                it->mWriteState(unit, out, it->mFieldName, hierarchy);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    StreamResult SerializeTable::readState(SerializableDataUnit *unit, FormattedSerializeStream &in, StateTransmissionFlags flags, CallerHierarchyBasePtr hierarchy) const
    {
        bool activation = flags & StateTransmissionFlags_Activation;
        if (activation) {
            setActive(unit, false, false);
        }

        StreamResult result = [&]() -> StreamResult {
            if (in.supportsNameLookup()) {
                std::string name;
                STREAM_PROPAGATE_ERROR(in.lookupFieldName(name));
                while (!name.empty()) {
                    bool found = false;
                    const SerializeTable *table = this;
                    while (table && !found) {
                        for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                            if (name == it->mFieldName) {
                                STREAM_PROPAGATE_ERROR(it->mReadState(unit, in, it->mFieldName, hierarchy));
                                found = true;
                                break;
                            }
                        }
                        table = table->mBaseType ? &table->mBaseType() : nullptr;
                    }
                    if (!found)
                        return STREAM_PARSE_ERROR(in.stream(), in.isBinary(), "Could not find field '" << name << "'");
                    STREAM_PROPAGATE_ERROR(in.lookupFieldName(name));
                }
            } else {
                const SerializeTable *table = this;
                while (table) {
                    for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                        STREAM_PROPAGATE_ERROR(it->mReadState(unit, in, it->mFieldName, hierarchy));
                    }
                    table = table->mBaseType ? &table->mBaseType() : nullptr;
                }
            }
            return {};
        }();

        if (flags & StateTransmissionFlags_ApplyMap) {
            assert(in.manager());
            STREAM_PROPAGATE_ERROR(applyMap(unit, in, result.mState == StreamState::OK));
        }

        if (activation) {
            setActive(unit, true, false);
        }

        return result;
    }

    StreamResult SerializeTable::readState(SerializableUnitBase *unit, FormattedSerializeStream &in, StateTransmissionFlags flags, CallerHierarchyBasePtr hierarchy) const
    {
        bool activation = flags & StateTransmissionFlags_Activation;
        if (activation) {
            setActive(unit, false, false);
        }

        StreamResult result = readState(static_cast<SerializableDataUnit *>(unit), in, flags & ~StateTransmissionFlags_Activation, hierarchy);

        if (activation) {
            setActive(unit, true, false);
        }

        return result;
    }

    StreamResult SerializeTable::readAction(SerializableUnitBase *unit, FormattedBufferedStream &in, PendingRequest *request) const
    {
        uint8_t index;
        STREAM_PROPAGATE_ERROR(read(in, index, "index"));
        return get(index).mReadAction(unit, in, request);
    }

    StreamResult SerializeTable::readRequest(SerializableUnitBase *unit, FormattedBufferedStream &inout, TransactionId id) const
    {
        uint8_t index;
        STREAM_PROPAGATE_ERROR(read(inout, index, "index"));        
        return get(index).mReadRequest(unit, inout, id);
    }

    StreamResult SerializeTable::applyMap(SerializableDataUnit *unit, FormattedSerializeStream &in, bool success) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                STREAM_PROPAGATE_ERROR(it->mApplySerializableMap(unit, in, success));
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
        return {};
    }

    void SerializeTable::setSynced(SerializableUnitBase *unit, bool b) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                it->mSetDataSynced(unit, b);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::setActive(SerializableDataUnit *unit, bool active, bool existenceChanged) const
    {
        if (!active && mCallbacks.onActivate)
            mCallbacks.onActivate(unit, active, existenceChanged);

        //TODO: Start with base
        const SerializeTable *table = this;
        while (table) {
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                it->mSetActive(unit, active, existenceChanged);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
        if (active && mCallbacks.onActivate)
            mCallbacks.onActivate(unit, active, existenceChanged);
    }

    void SerializeTable::setActive(SerializableUnitBase *unit, bool active, bool existenceChanged) const
    {
        if (active)
            assert(unit->mActiveIndex == 0);
        else if (mCallbacks.onActivate)
            mCallbacks.onActivate(unit, active, existenceChanged);

        //TODO: Start with base
        const SerializeTable *table = this;
        while (table) {
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                if (active)
                    ++unit->mActiveIndex;
                else {
                    assert(unit->mActiveIndex > 0);
                    --unit->mActiveIndex;
                }
                it->mSetActive(unit, active, existenceChanged);
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
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                it->mSetParent(unit);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::writeAction(const SerializableUnitBase *parent, uint8_t index, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data) const
    {
        for (FormattedBufferedStream &out : outStreams) {
            write(out, index, "index");
        }
        get(index).mWriteAction(parent, outStreams, data);
    }

    void SerializeTable::writeRequest(const SerializableUnitBase *parent, uint8_t index, FormattedBufferedStream &out, const void *data) const
    {
        write(out, index, "index");
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
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                if (it->mOffset() == offset) {
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
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                if (index == 0) {
                    return *it;
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