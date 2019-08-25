#pragma once

namespace Engine {
namespace Serialize {

    struct Serializer {
        const char *mFieldName;
        size_t (*mIndex)();

        void (*mWriteBinary)(const SerializableUnitBase *, SerializeOutStream &);
		void (*mReadBinary)(SerializableUnitBase *, SerializeInStream &);        

		void (*mWritePlain)(const SerializableUnitBase *, SerializeOutStream &, Formatter &);
        void (*mReadPlain)(SerializableUnitBase *, SerializeInStream &, Formatter &);

		void (*mReadAction)(SerializableUnitBase *, SerializeInStream &);
        void (*mReadRequest)(SerializableUnitBase *, BufferedInOutStream &);

		void (*mApplySerializableMap)(SerializableUnitBase *, const std::map<size_t, SerializableUnitBase *> &);
        void (*mSetDataSynced)(SerializableUnitBase *, bool);
        void (*mSetActive)(SerializableUnitBase *, bool);
    };

}
}