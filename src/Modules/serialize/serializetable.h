#pragma once

#include "serializer.h"

namespace Engine {
namespace Serialize {



    struct MODULES_EXPORT SerializeTable {   
		const char *mTypeName;
        const std::pair<const char *, Serializer> *mFields;
        bool mIsTopLevelUnit;

		void readBinary(SerializableUnitBase *unit, SerializeInStream &in) const;
        void writeBinary(const SerializableUnitBase *unit, SerializeOutStream &out) const;

		void readAction(SerializableUnitBase *unit, SerializeInStream &in, size_t index) const;
        void readRequest(SerializableUnitBase *unit, BufferedInOutStream &in, size_t index) const;

		void applySerializableMap(SerializableUnitBase *unit, const std::map<size_t, SerializableUnitBase *> &map) const;
        void setDataSynced(SerializableUnitBase *unit, bool b) const;
        void setActive(SerializableUnitBase *unit, bool b) const;
    };

}
}

DLL_IMPORT_VARIABLE2(const Engine::Serialize::SerializeTable, serializeTable, typename T);
