#include "../../moduleslib.h"

#include "serializestreambuf.h"

#include "../serializemanager.h"

namespace Engine {
namespace Serialize {

	SerializeStreambuf::SerializeStreambuf(std::unique_ptr<Formatter> format)
        : mFormatter(std::move(format))
    {
    }

    SerializeStreambuf::SerializeStreambuf(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id)
        : mManager(&mgr)
        , mId(id)
        , mFormatter(std::move(format))
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