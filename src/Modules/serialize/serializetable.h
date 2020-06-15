#pragma once

#include "statetransmissionflags.h"

namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION_ARGS(onActivate, int);
    DERIVE_FUNCTION_ARGS2(onActivate, onActivate2, int, int);

    struct SerializeTableCallbacks {
        template <typename T>
        constexpr SerializeTableCallbacks(type_holder_t<T>)
            : onActivate([](SerializableUnitBase *unit, int active, int existenceChanged) {
                if constexpr (has_function_onActivate_v<T> || has_function_onActivate2_v<T>)
                    TupleUnpacker::invoke(&T::onActivate, static_cast<T*>(unit), active, existenceChanged);
            })
        {

        }

        void (*onActivate)(SerializableUnitBase *, int, int);
    };

    struct MODULES_EXPORT SerializeTable {
        const char *mTypeName;
        SerializeTableCallbacks mCallbacks;
        const SerializeTable &(*mBaseType)();
        const std::pair<const char *, Serializer> *mFields;
        bool mIsTopLevelUnit;

        void writeState(const SerializableUnitBase *unit, SerializeOutStream &out) const;
        void readState(SerializableUnitBase *unit, SerializeInStream &in, StateTransmissionFlags flags = 0) const;

        void readAction(SerializableUnitBase *unit, SerializeInStream &in, PendingRequest *request) const;
        void readRequest(SerializableUnitBase *unit, BufferedInOutStream &in, TransactionId id) const;

        void applySerializableMap(SerializableUnitBase *unit, SerializeInStream &in) const;
        void setDataSynced(SerializableUnitBase *unit, bool b) const;
        void setActive(SerializableUnitBase *unit, bool active, bool existenceChanged) const;

        bool isInstance(SerializableUnitBase *unit) const;

        uint8_t getIndex(size_t offset) const;
        const Serializer &get(uint8_t index) const;
    };

}
}

DLL_IMPORT_VARIABLE2(const Engine::Serialize::SerializeTable, serializeTable, typename T);
