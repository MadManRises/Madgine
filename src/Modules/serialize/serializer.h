#pragma once

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        size_t (*mOffset)() = nullptr;

        void (*mWriteState)(const SerializableUnitBase *, SerializeOutStream &, const char *name) = nullptr;
        void (*mReadState)(SerializableUnitBase *, SerializeInStream &, const char *name) = nullptr;

        void (*mReadAction)(SerializableUnitBase *, SerializeInStream &, PendingRequest *) = nullptr;
        void (*mReadRequest)(SerializableUnitBase *, BufferedInOutStream &, TransactionId id) = nullptr;

        void (*mApplySerializableMap)(SerializableUnitBase *, SerializeInStream &) = nullptr;
        void (*mSetDataSynced)(SerializableUnitBase *, bool) = nullptr;
        void (*mSetActive)(SerializableUnitBase *, bool, bool) = nullptr;
    };

}
}