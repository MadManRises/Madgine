#include "../../metalib.h"

#include "serializestreambuf.h"

#include "../serializemanager.h"

#include "../formatter.h"

#include "serializablemapholder.h"

namespace Engine {
namespace Serialize {

    SerializeStreamData::SerializeStreamData(std::unique_ptr<Formatter> format)
        : mFormatter(std::move(format))
    {
    }

    SerializeStreamData::SerializeStreamData(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id)
        : mManager(&mgr)
        , mId(id)
        , mFormatter(std::move(format))
    {
    }

    SerializeStreamData::~SerializeStreamData()
    {
    }

    void SerializeStreamData::setManager(SerializeManager *mgr)
    {
        mManager = mgr;
    }

    SerializeManager *SerializeStreamData::manager()
    {
        return mManager;
    }

    Formatter &SerializeStreamData::format() const
    {
        return *mFormatter;
    }

    SerializableUnitList &SerializeStreamData::serializableList()
    {
        assert(mSerializableList);
        return *mSerializableList;
    }

    void SerializeStreamData::startSerializableRead(SerializableListHolder *list)
    {
        assert(!list->mData);
        if (!mSerializableList) {
            list->mData = this;
            mSerializableList = &list->mList;
        }
    }

    SerializableUnitMap &SerializeStreamData::serializableMap()
    {
        assert(mSerializableMap);
        return *mSerializableMap;
    }

    void SerializeStreamData::startSerializableWrite(SerializableMapHolder *map)
    {
        assert(!map->mData);
        if (!mSerializableMap) {
            map->mData = this;
            mSerializableMap = &map->mMap;
        }
    }

    bool SerializeStreamData::isMaster(StreamMode mode)
    {
        if (mManager)
            return mManager->isMaster(this);
        return mode == StreamMode::WRITE;
    }

    ParticipantId SerializeStreamData::id() const
    {
        return mId;
    }

    void SerializeStreamData::setId(ParticipantId id)
    {
        mId = id;
    }

}
}