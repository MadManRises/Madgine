#include "../moduleslib.h"

#include "serializetable.h"

#include "../keyvalue/keyvalue.h"

#include "formatter.h"

#include "streams/serializestream.h"

#include "serializableunit.h"

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

    void SerializeTable::readState(SerializableUnitBase *unit, SerializeInStream &in) const
    {
        Formatter &format = in.format();

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
                    throw 0;
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
    }

    void SerializeTable::readAction(SerializableUnitBase *unit, SerializeInStream &in, size_t index) const
    {
        get(index).mReadAction(unit, in);
    }

    void SerializeTable::readRequest(SerializableUnitBase *unit, BufferedInOutStream &inout, size_t index) const
    {
        get(index).mReadRequest(unit, inout);
    }

    void SerializeTable::applySerializableMap(SerializableUnitBase *unit, const std::map<size_t, SerializableUnitBase *> &map) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mApplySerializableMap(unit, map);
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

    void SerializeTable::setActive(SerializableUnitBase *unit, bool b) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mSetActive(unit, b);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    bool SerializeTable::isInstance(SerializableUnitBase *unit) const
    {
        return !unit || unit->mType == this;
    }

    size_t SerializeTable::getIndex(size_t offset) const
    {
        size_t index = 0;

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

        throw 0;
    }

    const Serializer &SerializeTable::get(size_t index) const
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
                if (index == 0){
                    return it->second;
                }
                --index;
            }
            tables.pop();
        }

        //Corrupt package
        throw 0;
    }

}
}