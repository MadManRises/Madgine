#pragma once

namespace Engine {
namespace Serialize {

    struct SerializableMapHolder {

        SerializableMapHolder() = default;     
        SerializableMapHolder(const SerializableMapHolder &) = delete;
        SerializableMapHolder(SerializableMapHolder &&) = delete;
        ~SerializableMapHolder();

        SerializableUnitMap mMap;    
        SerializeStreamData *mData = nullptr;
    };

    struct SerializableListHolder {

        SerializableListHolder() = default;
        SerializableListHolder(const SerializableListHolder &) = delete;
        SerializableListHolder(SerializableListHolder &&) = delete;
        ~SerializableListHolder();

        SerializableUnitList mList;
        SerializeStreamData *mData = nullptr;
    };

}
}