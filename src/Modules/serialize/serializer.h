#pragma once

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        size_t (*mIndex)();

        void (*mWriteState)(const SerializableUnitBase *, SerializeOutStream &);
		void (*mReadState)(SerializableUnitBase *, SerializeInStream &);        

		void (*mReadAction)(SerializableUnitBase *, SerializeInStream &);
        void (*mReadRequest)(SerializableUnitBase *, BufferedInOutStream &);

		void (*mApplySerializableMap)(SerializableUnitBase *, const std::map<size_t, SerializableUnitBase *> &);
        void (*mSetDataSynced)(SerializableUnitBase *, bool);
        void (*mSetActive)(SerializableUnitBase *, bool);
    };

}
}