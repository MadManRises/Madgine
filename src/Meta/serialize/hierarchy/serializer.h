#pragma once

#include "Generic/callerhierarchy.h"

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        OffsetPtr (*mOffset)() = nullptr;

        void (*mWriteState)(const SerializableDataUnit *, FormattedSerializeStream &, const char *, CallerHierarchyBasePtr) = nullptr;
        StreamResult (*mReadState)(SerializableDataUnit *, FormattedSerializeStream &, const char *, CallerHierarchyBasePtr) = nullptr;

        StreamResult (*mReadAction)(SyncableUnitBase *, FormattedBufferedStream &, PendingRequest &) = nullptr;
        StreamResult (*mReadRequest)(SyncableUnitBase *, FormattedBufferedStream &, MessageId) = nullptr;

        StreamResult (*mApplySerializableMap)(SerializableDataUnit *, FormattedSerializeStream &, bool) = nullptr;
        void (*mSetDataSynced)(SerializableDataUnit *, bool) = nullptr;
        void (*mSetActive)(SerializableDataUnit *, bool, bool) = nullptr;
        void (*mSetParent)(SerializableDataUnit *) = nullptr;

        void (*mWriteAction)(const SyncableUnitBase *, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *) = nullptr;
        void (*mWriteRequest)(const SyncableUnitBase *, FormattedBufferedStream &out, const void *) = nullptr;
    };

}
}