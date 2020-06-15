#include "../moduleslib.h"

#include "serializetable.h"

#include "formatter.h"

#include "streams/bufferedstream.h"

#include "serializableunit.h"

#include "serializer.h"

namespace Engine {
namespace Serialize {

    void SerializeTable::writeState(const SerializableUnitBase *unit, SerializeOutStream &out) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mWriteState(unit, out, it->second.mFieldName);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::readState(SerializableUnitBase *unit, SerializeInStream &in, StateTransmissionFlags flags) const
    {
        Formatter &format = in.format();

        bool wasActive = unit->mActiveIndex > 0; //Not exact, but active unit with 0 members doesn't matter
        if (wasActive) {
            setActive(unit, false, false);
        }

        if (format.mSupportNameLookup) {
            std::string name = format.lookupFieldName(in);
            while (!name.empty()) {
                bool found = false;
                const SerializeTable *table = this;
                while (table && !found) {
                    for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                        if (name == it->second.mFieldName) {
                            it->second.mReadState(unit, in, it->second.mFieldName);
                            found = true;
                            break;
                        }
                    }
                    table = table->mBaseType ? &table->mBaseType() : nullptr;
                }
                if (!found)
                    std::terminate();
                name = format.lookupFieldName(in);
            }
        } else {
            const SerializeTable *table = this;
            while (table) {
                for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                    it->second.mReadState(unit, in, it->second.mFieldName);
                }
                table = table->mBaseType ? &table->mBaseType() : nullptr;
            }
        }

		if (!(flags & StateTransmissionFlags_DontApplyMap)) {
            assert(in.manager());
            applySerializableMap(unit, in);
        }

		if (wasActive) {
            setActive(unit, true, false);
		}
    }

    void SerializeTable::readAction(SerializableUnitBase *unit, SerializeInStream &in, PendingRequest *request) const
    {
        uint8_t index;
        in >> index;
        get(index).mReadAction(unit, in, request);
    }

    void SerializeTable::readRequest(SerializableUnitBase *unit, BufferedInOutStream &inout, TransactionId id) const
    {
        uint8_t index;
        inout >> index;
        get(index).mReadRequest(unit, inout, id);
    }

    void SerializeTable::applySerializableMap(SerializableUnitBase *unit, SerializeInStream &in) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mApplySerializableMap(unit, in);
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

    bool SerializeTable::isInstance(SerializableUnitBase *unit) const
    {
        return !unit || unit->mType == this;
    }

    uint8_t SerializeTable::getIndex(size_t offset) const
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

        std::terminate();
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