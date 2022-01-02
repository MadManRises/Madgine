#include "../interfaceslib.h"

#include "log.h"
#include "standardlog.h"

namespace Engine {
namespace Util {

    Log *sLog = nullptr;

    void setLog(Log *log)
    {
        sLog = log;
    }

    void log(std::string_view msg, MessageType level, const char *file)
    {
        if (sLog)
            sLog->log(msg, level, file);
        else
            StandardLog::sLog(msg, level);
    }

    LogDummy::LogDummy(MessageType lvl, const char *file)
        : mLvl(lvl)
        , mFile(file)
    {
    }

    LogDummy::~LogDummy()
    {
        log(mStream.str(), mLvl, mFile);
    }

}
}
