#include "../../metalib.h"

#include "serializer.h"
#include "serializetable.h"
#include "syncfunction.h"

#include "statetransmissionflags.h"

#include "serializableunit.h"
#include "syncableunit.h"

#include "../operations.h"

#include "../streams/formattedbufferedstream.h"

#include "Generic/offsetptr.h"

#include "../streams/comparestreamid.h"

namespace Engine {
namespace Serialize {

    void writeFunctionAction(SyncableUnitBase *unit, uint16_t index, const void *args, const std::set<ParticipantId> &targets, ParticipantId answerTarget, MessageId answerId)
    {
        unit->writeFunctionAction(index, args, targets, answerTarget, answerId);
    }

    void writeFunctionResult(SyncableUnitBase *unit, uint16_t index, const void *result, FormattedBufferedStream &target, MessageId answerId)
    {
        unit->writeFunctionResult(index, result, target, answerId);
    }

    void writeFunctionRequest(SyncableUnitBase *unit, uint16_t index, FunctionType type, const void *args, ParticipantId requester, MessageId requesterTransactionId, GenericMessageReceiver receiver)
    {
        unit->writeFunctionRequest(index, type, args, requester, requesterTransactionId, std::move(receiver));
    }

    void writeFunctionError(SyncableUnitBase *unit, uint16_t index, MessageResult error, FormattedBufferedStream &target, MessageId answerId)
    {
        unit->writeFunctionError(index, error, target, answerId);
    }

    StreamResult META_EXPORT readState(const SerializeTable *table, SerializableDataUnit *unit, FormattedSerializeStream &in, StateTransmissionFlags flags, CallerHierarchyBasePtr hierarchy)
    {
        if (in.supportsNameLookup()) {
            std::string name;
            STREAM_PROPAGATE_ERROR(in.lookupFieldName(name));
            while (!name.empty()) {
                bool found = false;
                const SerializeTable *tableAcc = table;
                while (tableAcc && !found) {
                    for (const Serializer *it = tableAcc->mFields; it->mFieldName; ++it) {
                        if (name == it->mFieldName) {
                            STREAM_PROPAGATE_ERROR(it->mReadState(unit, in, it->mFieldName, hierarchy));
                            found = true;
                            break;
                        }
                    }
                    tableAcc = tableAcc->mBaseType ? &tableAcc->mBaseType() : nullptr;
                }
                if (!found)
                    return STREAM_PARSE_ERROR(in) << "Could not find field '" << name << "'";
                STREAM_PROPAGATE_ERROR(in.lookupFieldName(name));
            }
        } else {
            const SerializeTable *tableAcc = table;
            while (tableAcc) {
                for (const Serializer *it = tableAcc->mFields; it->mFieldName; ++it) {
                    STREAM_PROPAGATE_ERROR(it->mReadState(unit, in, it->mFieldName, hierarchy));
                }
                tableAcc = tableAcc->mBaseType ? &tableAcc->mBaseType() : nullptr;
            }
        }
        return {};
    }

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
        return mReadState(this, unit, in, flags, hierarchy);
    }

    StreamResult SerializeTable::readAction(SerializableDataUnit *unit, FormattedBufferedStream &in, PendingRequest &request) const
    {
        uint16_t index;
        STREAM_PROPAGATE_ERROR(read(in, index, "index"));
        return get(index).mReadAction(unit, in, request);
    }

    StreamResult SerializeTable::readRequest(SerializableDataUnit *unit, FormattedBufferedStream &inout, MessageId id) const
    {
        uint16_t index;
        STREAM_PROPAGATE_ERROR(read(inout, index, "index"));
        return get(index).mReadRequest(unit, inout, id);
    }

    StreamResult SerializeTable::applyMap(SerializableDataUnit *unit, FormattedSerializeStream &in, bool success, CallerHierarchyBasePtr hierarchy) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const Serializer *it = table->mFields; it->mFieldName; ++it) {
                STREAM_PROPAGATE_ERROR(it->mApplySerializableMap(unit, in, success, hierarchy));
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

    void SerializeTable::writeAction(const SerializableDataUnit *unit, uint16_t index, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, void *data) const
    {
        for (FormattedBufferedStream &out : outStreams) {
            write(out, index, "index");
        }
        get(index).mWriteAction(unit, outStreams, data);
    }

    void SerializeTable::writeRequest(const SerializableDataUnit *unit, uint16_t index, FormattedBufferedStream &out, void *data) const
    {
        write(out, index, "index");
        get(index).mWriteRequest(unit, out, data);
    }

    uint16_t SerializeTable::getIndex(OffsetPtr offset) const
    {
        uint16_t index = 0;

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

    const Serializer &SerializeTable::get(uint16_t index) const
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

    const SyncFunction &SerializeTable::getFunction(uint16_t index) const
    {
        std::stack<const SerializeTable *> tables;
        const SerializeTable *table = this;
        while (table) {
            tables.push(table);
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }

        while (!tables.empty()) {
            table = tables.top();
            for (const SyncFunction *it = table->mFunctions; true; ++it) {
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

    void SerializeTable::writeFunctionArguments(const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, uint16_t index, FunctionType type, const void *args) const
    {
        for (FormattedBufferedStream &out : outStreams) {
            write(out, index, "index");
            write(out, type, "functionType");
        }
        getFunction(index).mWriteFunctionArguments(outStreams, args);
    }

    void SerializeTable::writeFunctionResult(FormattedBufferedStream &out, uint16_t index, const void *args) const
    {
        write(out, index, "index");
        write(out, QUERY, "functionType");
        getFunction(index).mWriteFunctionResult(out, args);
    }

    void SerializeTable::writeFunctionError(FormattedBufferedStream &out, uint16_t index, MessageResult error) const
    {
        write(out, error, "error");
    }

    StreamResult SerializeTable::readFunctionAction(SyncableUnitBase *unit, FormattedBufferedStream &in, PendingRequest &request) const
    {
        uint16_t index;
        STREAM_PROPAGATE_ERROR(read(in, index, "index"));
        FunctionType type;
        STREAM_PROPAGATE_ERROR(read(in, type, "functionType"));
        return getFunction(index).mReadFunctionAction(unit, in, index, type, request);
    }

    StreamResult SerializeTable::readFunctionRequest(SyncableUnitBase *unit, FormattedBufferedStream &in, MessageId id) const
    {
        uint16_t index;
        STREAM_PROPAGATE_ERROR(read(in, index, "index"));
        FunctionType type;
        STREAM_PROPAGATE_ERROR(read(in, type, "functionType"));
        return getFunction(index).mReadFunctionRequest(unit, in, index, type, id);
    }

    StreamResult SerializeTable::readFunctionError(SyncableUnitBase *unit, FormattedBufferedStream &in, PendingRequest &request) const
    {
        MessageResult error;
        STREAM_PROPAGATE_ERROR(read(in, error, "error"));
        request.mReceiver.set_error(error);
        return {};
    }

}
}