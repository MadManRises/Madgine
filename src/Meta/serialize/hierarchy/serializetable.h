#pragma once

#include "serializetable_forward.h"

#include "Generic/callerhierarchy.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Serialize {

    struct SerializeTableCallbacks {
        template <typename T>
        constexpr SerializeTableCallbacks(type_holder_t<T>)
            : onActivate([](SerializableDataUnit *unit, bool active, bool existenceChanged) {
                if constexpr (has_function_onActivate_v<T, bool, bool>)
                    static_cast<T *>(unit)->onActivate(active, existenceChanged);
                else if constexpr (has_function_onActivate_v<T, bool>)
                    static_cast<T *>(unit)->onActivate(active);
                else if constexpr (has_function_onActivate_v<T>)
                    static_cast<T *>(unit)->onActivate();
            })
        {
        }

        void (*onActivate)(SerializableDataUnit *, bool, bool);
    };

    struct META_EXPORT SerializeTable {
        const char *mTypeName;
        SerializeTableCallbacks mCallbacks;
        const SerializeTable &(*mBaseType)();
        StreamResult (*mReadState)(const SerializeTable *, SerializableDataUnit *, FormattedSerializeStream &, StateTransmissionFlags, CallerHierarchyBasePtr);
        const Serializer *mFields;
        const SyncFunction *mFunctions;
        bool mIsTopLevelUnit;

        void writeState(const SerializableDataUnit *unit, FormattedSerializeStream &out, CallerHierarchyBasePtr hierarchy = {}) const;
        StreamResult readState(SerializableDataUnit *unit, FormattedSerializeStream &in, StateTransmissionFlags flags = 0, CallerHierarchyBasePtr hierarchy = {}) const;

        StreamResult readAction(SerializableDataUnit *unit, FormattedBufferedStream &in, PendingRequest &request) const;
        StreamResult readRequest(SerializableDataUnit *unit, FormattedBufferedStream &in, MessageId id) const;

        StreamResult applyMap(SerializableDataUnit *unit, FormattedSerializeStream &in, bool success, CallerHierarchyBasePtr hierarchy) const;
        void setSynced(SerializableUnitBase *unit, bool b) const;
        void setActive(SerializableDataUnit *unit, bool active, bool existenceChanged) const;
        void setActive(SerializableUnitBase *unit, bool active, bool existenceChanged) const;
        void setParent(SerializableUnitBase *unit) const;

        void writeAction(const SerializableDataUnit *unit, uint16_t index, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, void *data) const;
        void writeRequest(const SerializableDataUnit *unit, uint16_t index, FormattedBufferedStream &out, void *data) const;

        uint16_t getIndex(OffsetPtr offset) const;
        const Serializer &get(uint16_t index) const;

        const SyncFunction &getFunction(uint16_t index) const;

        void writeFunctionArguments(const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, uint16_t index, FunctionType type, const void *args) const;
        void writeFunctionResult(FormattedBufferedStream &out, uint16_t index, const void *args) const;
        void writeFunctionError(FormattedBufferedStream &out, uint16_t index, MessageResult error) const;
        StreamResult readFunctionAction(SyncableUnitBase *unit, FormattedBufferedStream &in, PendingRequest &request) const;
        StreamResult readFunctionRequest(SyncableUnitBase *unit, FormattedBufferedStream &in, MessageId id) const;
        StreamResult readFunctionError(SyncableUnitBase *unit, FormattedBufferedStream &in, PendingRequest &request) const;
    };

}
}
