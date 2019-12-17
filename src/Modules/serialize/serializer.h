#pragma once

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        size_t (*mOffset)();

        void (*mWriteState)(const SerializableUnitBase *, SerializeOutStream &, const char *name);
        void (*mReadState)(SerializableUnitBase *, SerializeInStream &, const char *name);        

		void (*mReadAction)(SerializableUnitBase *, SerializeInStream &);
        void (*mReadRequest)(SerializableUnitBase *, BufferedInOutStream &);

		void (*mApplySerializableMap)(SerializableUnitBase *, const std::map<size_t, SerializableUnitBase *> &);
        void (*mSetDataSynced)(SerializableUnitBase *, bool);
        void (*mSetActive)(SerializableUnitBase *, bool, bool);
    };

}
}