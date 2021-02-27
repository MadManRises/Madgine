#pragma once

#include "statetransmissionflags.h"
#include "Generic/callerhierarchy.h"

namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION(onActivate);    

    struct SerializeTableCallbacks {
        template <typename T>
        constexpr SerializeTableCallbacks(type_holder_t<T>)
            : onActivate([](SerializableUnitBase *unit, int active, int existenceChanged) {
                if constexpr (has_function_onActivate_upTo_v<T, 2>)
                    TupleUnpacker::invoke(&T::onActivate, static_cast<T*>(unit), active, existenceChanged);
            })
        {

        }

        void (*onActivate)(SerializableUnitBase *, int, int);
    };

    struct META_EXPORT SerializeTable {
        const char *mTypeName;
        SerializeTableCallbacks mCallbacks;
        const SerializeTable &(*mBaseType)();
        const std::pair<const char *, Serializer> *mFields;
        bool mIsTopLevelUnit;

        void writeState(const SerializableDataUnit *unit, SerializeOutStream &out, CallerHierarchyBasePtr hierarchy = {}) const;
        void readState(SerializableDataUnit *unit, SerializeInStream &in, StateTransmissionFlags flags = 0, CallerHierarchyBasePtr hierarchy = {}) const;
        void readState(SerializableUnitBase *unit, SerializeInStream &in, StateTransmissionFlags flags = 0, CallerHierarchyBasePtr hierarchy = {}) const;

        void readAction(SerializableUnitBase *unit, SerializeInStream &in, PendingRequest *request) const;
        void readRequest(SerializableUnitBase *unit, BufferedInOutStream &in, TransactionId id) const;

        void applySerializableMap(SerializableDataUnit *unit, SerializeInStream &in) const;
        void setDataSynced(SerializableUnitBase *unit, bool b) const;
        void setActive(SerializableUnitBase *unit, bool active, bool existenceChanged) const;
        void setParent(SerializableUnitBase *unit) const;

        void writeAction(const SerializableUnitBase *parent, uint8_t index, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *data) const;
        void writeRequest(const SerializableUnitBase *parent, uint8_t index, BufferedOutStream &out, const void *data) const;

        uint8_t getIndex(size_t offset) const;
        const Serializer &get(uint8_t index) const;
    };

}
}

DLL_IMPORT_VARIABLE2(const Engine::Serialize::SerializeTable, serializeTable, typename T);
