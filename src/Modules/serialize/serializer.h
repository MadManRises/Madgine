#pragma once

#include "../generic/callerhierarchy.h"

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        size_t (*mOffset)() = nullptr;

        void (*mWriteState)(const SerializableDataUnit *, SerializeOutStream &, const char *, CallerHierarchyBasePtr) = nullptr;
        void (*mReadState)(SerializableDataUnit *, SerializeInStream &, const char *, CallerHierarchyBasePtr) = nullptr;

        void (*mReadAction)(SerializableDataUnit *, SerializeInStream &, PendingRequest *) = nullptr;
        void (*mReadRequest)(SerializableDataUnit *, BufferedInOutStream &, TransactionId) = nullptr;

        void (*mApplySerializableMap)(SerializableDataUnit *, SerializeInStream &) = nullptr;
        void (*mSetDataSynced)(SerializableDataUnit *, bool) = nullptr;
        void (*mSetActive)(SerializableDataUnit *, bool, bool) = nullptr;
        void (*mSetParent)(SerializableDataUnit *) = nullptr;

        void (*mWriteAction)(const SerializableDataUnit *, int, const void *, const std::set<BufferedOutStream *, CompareStreamId> &outStreams) = nullptr;
        void (*mWriteRequest)(const SerializableDataUnit *, int, const void *, BufferedOutStream *out) = nullptr;
    };

}
}