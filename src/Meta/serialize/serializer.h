#pragma once

#include "Generic/callerhierarchy.h"

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        size_t (*mOffset)() = nullptr;

        void (*mWriteState)(const SerializableDataUnit *, SerializeOutStream &, const char *, CallerHierarchyBasePtr) = nullptr;
        StreamResult (*mReadState)(SerializableDataUnit *, SerializeInStream &, const char *, CallerHierarchyBasePtr) = nullptr;

        StreamResult (*mReadAction)(SerializableDataUnit *, SerializeInStream &, PendingRequest *) = nullptr;
        StreamResult (*mReadRequest)(SerializableDataUnit *, BufferedInOutStream &, TransactionId) = nullptr;

        void (*mApplySerializableMap)(SerializableDataUnit *, SerializeInStream &, bool) = nullptr;
        void (*mSetDataSynced)(SerializableDataUnit *, bool) = nullptr;
        void (*mSetActive)(SerializableDataUnit *, bool, bool) = nullptr;
        void (*mSetParent)(SerializableDataUnit *) = nullptr;

        void (*mWriteAction)(const SerializableDataUnit *, const std::set<BufferedOutStream *, CompareStreamId> &outStreams, const void *) = nullptr;
        void (*mWriteRequest)(const SerializableDataUnit *, BufferedOutStream &out, const void *) = nullptr;
    };

}
}