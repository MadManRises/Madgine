#pragma once

namespace Engine {
namespace Serialize {

    struct SerializableMapHolder {

        SerializableMapHolder(SerializeOutStream &out);     
        SerializableMapHolder(const SerializableMapHolder &) = delete;
        SerializableMapHolder(SerializableMapHolder &&) = delete;
        ~SerializableMapHolder();

        SerializableUnitMap mMap;    
        SerializeStreamData *mData = nullptr;
    };

    struct SerializableListHolder {

        SerializableListHolder(SerializeInStream &in);
        SerializableListHolder(const SerializableListHolder &) = delete;
        SerializableListHolder(SerializableListHolder &&) = delete;
        ~SerializableListHolder();

        SerializableUnitList mList;
        SerializeStreamData *mData = nullptr;
    };

}
}