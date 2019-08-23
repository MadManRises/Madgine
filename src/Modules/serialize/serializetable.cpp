#include "../moduleslib.h"

#include "serializetable.h"

#include "../keyvalue/keyvalue.h"

namespace Engine {
namespace Serialize {

	void SerializeTable::readBinary(SerializableUnitBase *unit, SerializeInStream &in) const
    {
        for (const std::pair<const char *, Serializer> *it = mFields; it->first; ++it) {
            it->second.mReadBinary(unit, in);
        }
    }

    void SerializeTable::writeBinary(const SerializableUnitBase *unit, SerializeOutStream &out) const
    {
        for (const std::pair<const char *, Serializer> *it = mFields; it->first; ++it) {        
            it->second.mWriteBinary(unit, out);
        }
    }

	void SerializeTable::readAction(SerializableUnitBase *unit, SerializeInStream &in, size_t index) const
    {
            //reinterpret_cast<ObservableBase *>(reinterpret_cast<char *>(this) + index);
            for (const std::pair<const char *, Serializer> *it = mFields; it->first; ++it) {
                if (it->second.mIndex() == index) {
                    it->second.mReadAction(unit, in);
                    return;
                }
            }
			//Corrupt package
            throw 0;
    }

    void SerializeTable::readRequest(SerializableUnitBase *unit, BufferedInOutStream &inout, size_t index) const
    {
        //reinterpret_cast<ObservableBase *>(reinterpret_cast<char *>(this) + index);
        for (const std::pair<const char *, Serializer> *it = mFields; it->first; ++it) {
            if (it->second.mIndex() == index) {
                it->second.mReadRequest(unit, inout);
                return;
            }
        }
		//Corrupt package
        throw 0;
    }

    void SerializeTable::applySerializableMap(SerializableUnitBase *unit, const std::map<size_t, SerializableUnitBase *> &map) const
    {
        for (const std::pair<const char *, Serializer> *it = mFields; it->first; ++it) {
            it->second.mApplySerializableMap(unit, map);
		}
    }

    void SerializeTable::setDataSynced(SerializableUnitBase *unit, bool b) const
    {
        for (const std::pair<const char *, Serializer> *it = mFields; it->first; ++it) {
            it->second.mSetDataSynced(unit, b);
        }
    }

    void SerializeTable::setActive(SerializableUnitBase *unit, bool b) const
    {
        for (const std::pair<const char *, Serializer> *it = mFields; it->first; ++it) {
            it->second.mSetActive(unit, b);
        }
    }

}
}