#include "../../metalib.h"

#include "serializestreambuf.h"

#include "../serializemanager.h"

#include "../formatter.h"

#include "serializablemapholder.h"

namespace Engine {
namespace Serialize {

    SerializeStreambuf::SerializeStreambuf(std::unique_ptr<Formatter> format)
        : mFormatter(std::move(format))
    {
    }

    SerializeStreambuf::SerializeStreambuf(SerializeStreambuf &&)
    {
    }

    SerializeStreambuf::SerializeStreambuf(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id)
        : mManager(&mgr)
        , mId(id)
        , mFormatter(std::move(format))
    {
    }

    SerializeStreambuf::~SerializeStreambuf()
    {
    }

    void SerializeStreambuf::setManager(SerializeManager *mgr)
    {
        mManager = mgr;
    }

    SerializeManager *SerializeStreambuf::manager()
    {
        return mManager;
    }

    Formatter &SerializeStreambuf::format() const
    {
        return *mFormatter;
    }

    SerializableUnitList &SerializeStreambuf::serializableList()
    {
        assert(mSerializableList);
        return *mSerializableList;
    }

    void SerializeStreambuf::startSerializableRead(SerializableListHolder *list)
    {
        assert(!list->mBuffer);
        if (!mSerializableList) {
            list->mBuffer = this;
            mSerializableList = &list->mList;
        }
    }

    SerializableUnitMap &SerializeStreambuf::serializableMap()
    {
        assert(mSerializableMap);
        return *mSerializableMap;
    }

    void SerializeStreambuf::startSerializableWrite(SerializableMapHolder *map)
    {
        assert(!map->mBuffer);
        if (!mSerializableMap) {
            map->mBuffer = this;
            mSerializableMap = &map->mMap;
        }
    }

    bool SerializeStreambuf::isMaster(StreamMode mode)
    {
        if (mManager)
            return mManager->isMaster(this);
        return mode == StreamMode::WRITE;
    }

    ParticipantId SerializeStreambuf::id() const
    {
        return mId;
    }

    void SerializeStreambuf::setId(ParticipantId id)
    {
        mId = id;
    }

}
}