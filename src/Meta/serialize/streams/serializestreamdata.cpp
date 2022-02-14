#include "../../metalib.h"

#include "serializestreamdata.h"

#include "../serializemanager.h"

#include "../formatter.h"

namespace Engine {
namespace Serialize {

    SerializeStreamData::SerializeStreamData()
    {
    }

    SerializeStreamData::SerializeStreamData(SerializeManager &mgr, ParticipantId id)
        : mManager(&mgr)
        , mId(id)
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

    SerializableUnitList &SerializeStreamData::serializableList()
    {
        assert(mSerializableList);
        return *mSerializableList;
    }

    SerializableUnitMap &SerializeStreamData::serializableMap()
    {
        assert(mSerializableMap);
        return *mSerializableMap;
    }

    bool SerializeStreamData::isMaster(StreamMode mode)
    {
        if (mManager)
            return mManager->isMaster(*this);
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