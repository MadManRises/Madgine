#pragma once

namespace Engine {
namespace Serialize {

    struct SerializableMapHolder {

        SerializableMapHolder(FormattedSerializeStream &out);     
        SerializableMapHolder(const SerializableMapHolder &) = delete;
        SerializableMapHolder(SerializableMapHolder &&) = delete;
        ~SerializableMapHolder();

        SerializableUnitMap mMap;    
        SerializeStreamData *mData = nullptr;
    };

    struct SerializableListHolder {

        SerializableListHolder(FormattedSerializeStream &in);
        SerializableListHolder(const SerializableListHolder &) = delete;
        SerializableListHolder(SerializableListHolder &&) = delete;
        ~SerializableListHolder();

        SerializableUnitList mList;
        SerializeStreamData *mData = nullptr;
    };

}
}