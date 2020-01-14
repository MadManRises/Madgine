#pragma once

#include "statetransmissionflags.h"

namespace Engine {
namespace Serialize {

    struct MODULES_EXPORT SerializeTable {
        const char *mTypeName;
        const SerializeTable &(*mBaseType)();
        const std::pair<const char *, Serializer> *mFields;
        bool mIsTopLevelUnit;

        void writeState(const SerializableUnitBase *unit, SerializeOutStream &out) const;
        void readState(SerializableUnitBase *unit, SerializeInStream &in, StateTransmissionFlags flags = 0) const;

        void readAction(SerializableUnitBase *unit, SerializeInStream &in) const;
        void readRequest(SerializableUnitBase *unit, BufferedInOutStream &in) const;

        void applySerializableMap(SerializableUnitBase *unit, SerializeInStream &in) const;
        void setDataSynced(SerializableUnitBase *unit, bool b) const;
        void setActive(SerializableUnitBase *unit, bool active, bool existenceChanged) const;

        bool isInstance(SerializableUnitBase *unit) const;

        size_t getIndex(size_t offset) const;
        const Serializer &get(size_t index) const;
    };

}
}

DLL_IMPORT_VARIABLE2(const Engine::Serialize::SerializeTable, serializeTable, typename T);
