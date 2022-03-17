#pragma once

namespace Engine {
namespace Serialize {

    struct META_EXPORT SerializableMapHolder {

        SerializableMapHolder(FormattedSerializeStream &out);     
        SerializableMapHolder(const SerializableMapHolder &) = delete;
        SerializableMapHolder(SerializableMapHolder &&) = delete;
        ~SerializableMapHolder();

        SerializableUnitMap mMap;    
        SerializeStreamData *mData = nullptr;
    };

    struct META_EXPORT SerializableListHolder {

        SerializableListHolder(FormattedSerializeStream &in);
        SerializableListHolder(const SerializableListHolder &) = delete;
        SerializableListHolder(SerializableListHolder &&) = delete;
        ~SerializableListHolder();

        SerializableUnitList mList;
        SerializeStreamData *mData = nullptr;
    };

}
}