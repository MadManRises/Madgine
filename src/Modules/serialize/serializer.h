#pragma once

namespace Engine {
namespace Serialize {

    struct Serializer {
        size_t (*mIndex)();

        void (*mReadBinary)(SerializableUnitBase *, SerializeInStream &);
        void (*mWriteBinary)(const SerializableUnitBase *, SerializeOutStream &);

		void (*mReadAction)(SerializableUnitBase *, SerializeInStream &);
        void (*mReadRequest)(SerializableUnitBase *, BufferedInOutStream &);

		void (*mReadText)(SerializableUnitBase *, SerializeInStream &);        
        void (*mWriteText)(SerializableUnitBase *, SerializeOutStream &);

		void (*mApplySerializableMap)(SerializableUnitBase *, const std::map<size_t, SerializableUnitBase *> &);
        void (*mSetDataSynced)(SerializableUnitBase *, bool);
        void (*mSetActive)(SerializableUnitBase *, bool);
    };

}
}