#pragma once

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        size_t (*mOffset)();

        void (*mWriteState)(const SerializableUnitBase *, SerializeOutStream &, const char *name);
        void (*mReadState)(SerializableUnitBase *, SerializeInStream &, const char *name);        

		void (*mReadAction)(SerializableUnitBase *, SerializeInStream &, PendingRequest *);
        void (*mReadRequest)(SerializableUnitBase *, BufferedInOutStream &, TransactionId id);

		void (*mApplySerializableMap)(SerializableUnitBase *, SerializeInStream &);
        void (*mSetDataSynced)(SerializableUnitBase *, bool);
        void (*mSetActive)(SerializableUnitBase *, bool, bool);
    };

}
}