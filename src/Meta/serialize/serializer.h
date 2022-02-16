#pragma once

#include "Generic/callerhierarchy.h"
#include "Generic/offsetptr.h"

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        OffsetPtr (*mOffset)() = nullptr;

        void (*mWriteState)(const SerializableDataUnit *, FormattedSerializeStream &, const char *, CallerHierarchyBasePtr) = nullptr;
        StreamResult (*mReadState)(SerializableDataUnit *, FormattedSerializeStream &, const char *, CallerHierarchyBasePtr) = nullptr;

        StreamResult (*mReadAction)(SerializableDataUnit *, FormattedBufferedStream &, PendingRequest *) = nullptr;
        StreamResult (*mReadRequest)(SerializableDataUnit *, FormattedBufferedStream &, TransactionId) = nullptr;

        StreamResult (*mApplySerializableMap)(SerializableDataUnit *, FormattedSerializeStream &, bool) = nullptr;
        void (*mSetDataSynced)(SerializableDataUnit *, bool) = nullptr;
        void (*mSetActive)(SerializableDataUnit *, bool, bool) = nullptr;
        void (*mSetParent)(SerializableDataUnit *) = nullptr;

        void (*mWriteAction)(const SerializableDataUnit *, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *) = nullptr;
        void (*mWriteRequest)(const SerializableDataUnit *, FormattedBufferedStream &out, const void *) = nullptr;
    };

}
}