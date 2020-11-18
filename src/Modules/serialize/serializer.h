#pragma once

#include "../generic/callerhierarchy.h"

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        size_t (*mOffset)() = nullptr;

        void (*mWriteState)(const SerializableUnitBase *, SerializeOutStream &, const char *, CallerHierarchyBasePtr) = nullptr;
        void (*mReadState)(SerializableUnitBase *, SerializeInStream &, const char *, CallerHierarchyBasePtr) = nullptr;

        void (*mReadAction)(SerializableUnitBase *, SerializeInStream &, PendingRequest *) = nullptr;
        void (*mReadRequest)(SerializableUnitBase *, BufferedInOutStream &, TransactionId) = nullptr;

        void (*mApplySerializableMap)(SerializableUnitBase *, SerializeInStream &) = nullptr;
        void (*mSetDataSynced)(SerializableUnitBase *, bool) = nullptr;
        void (*mSetActive)(SerializableUnitBase *, bool, bool) = nullptr;
        void (*mSetParent)(SerializableUnitBase *) = nullptr;

        void (*mWriteAction)(const SerializableUnitBase *, int, const void *, ParticipantId, TransactionId) = nullptr;
        void (*mWriteRequest)(const SerializableUnitBase *, int, const void *, ParticipantId, TransactionId, std::function<void(void *)>) = nullptr;
    };

}
}