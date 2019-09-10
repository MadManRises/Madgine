#include "../moduleslib.h"

#include "serializetable.h"

#include "../keyvalue/keyvalue.h"

#include "formatter.h"

#include "streams/serializestream.h"

namespace Engine {
namespace Serialize {

    void SerializeTable::writeState(const SerializableUnitBase *unit, SerializeOutStream &out) const
    {
        Formatter &format = out.format();

		bool first = true;
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                format.beginMember(out, it->second.mFieldName, first);
                it->second.mWriteState(unit, out);
                format.endMember(out, it->second.mFieldName, first);
                first = false;
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::readState(SerializableUnitBase *unit, SerializeInStream &in) const
    {
        Formatter &format = in.format();

        if (format.mSupportNameLookup) {
            std::string name = format.lookupFieldName(in);
            bool first = true;
            while (!name.empty()) {
                bool found = false;
                const SerializeTable *table = this;
                while (table && !found) {
                    for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                        if (name == it->second.mFieldName) {
                            format.beginMember(in, it->second.mFieldName, first);
                            it->second.mReadState(unit, in);
                            format.endMember(in, it->second.mFieldName, first);
                            first = false;
                            found = true;
                            break;
                        }
                    }
                    table = table->mBaseType ? &table->mBaseType() : nullptr;
                }
                name = format.lookupFieldName(in);
            }
        } else {
            bool first = true;

            const SerializeTable *table = this;
            while (table) {
                for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                    format.beginMember(in, it->second.mFieldName, first);
                    it->second.mReadState(unit, in);
                    format.endMember(in, it->second.mFieldName, first);
                    first = false;
                }
                table = table->mBaseType ? &table->mBaseType() : nullptr;
            }
        }
    }

    void SerializeTable::readAction(SerializableUnitBase *unit, SerializeInStream &in, size_t index) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                if (it->second.mIndex() == index) {
                    it->second.mReadAction(unit, in);
                    return;
                }
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
        //Corrupt package
        throw 0;
    }

    void SerializeTable::readRequest(SerializableUnitBase *unit, BufferedInOutStream &inout, size_t index) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                if (it->second.mIndex() == index) {
                    it->second.mReadRequest(unit, inout);
                    return;
                }
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
        //Corrupt package
        throw 0;
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

}
}