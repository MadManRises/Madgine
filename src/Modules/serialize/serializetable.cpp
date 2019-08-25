#include "../moduleslib.h"

#include "serializetable.h"

#include "../keyvalue/keyvalue.h"

#include "formatter.h"

namespace Engine {
namespace Serialize {

    void SerializeTable::writeBinary(const SerializableUnitBase *unit, SerializeOutStream &out) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mWriteBinary(unit, out);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::readBinary(SerializableUnitBase *unit, SerializeInStream &in) const
    {
        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                it->second.mReadBinary(unit, in);
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::writePlain(const SerializableUnitBase *unit, SerializeOutStream &out, Formatter &format, bool emitObjectHeader) const
    {
        bool first = true;

        const SerializeTable *table = this;
        while (table) {
            for (const std::pair<const char *, Serializer> *it = table->mFields; it->first; ++it) {
                format.beginMember(out, it->second.mFieldName, first);
                it->second.mWritePlain(unit, out, format);
                format.endMember(out, it->second.mFieldName, first);
                first = false;
            }
            table = table->mBaseType ? &table->mBaseType() : nullptr;
        }
    }

    void SerializeTable::readPlain(SerializableUnitBase *unit, SerializeInStream &in, Formatter &format, bool emitObjectHeader) const
    {
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
                            it->second.mReadPlain(unit, in, format);
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
                    it->second.mReadPlain(unit, in, format);
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
            //reinterpret_cast<ObservableBase *>(reinterpret_cast<char *>(this) + index);
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