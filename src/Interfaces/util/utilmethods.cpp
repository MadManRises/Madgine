#include "../interfaceslib.h"

#include "log.h"
#include "standardlog.h"

#if WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif

namespace Engine {
namespace Util {

    Log *sLog = nullptr;

    void setLog(Log *log)
    {
        sLog = log;
    }

    void log(std::string_view msg, MessageType level, const char *file, size_t line)
    {
        if (sLog)
            sLog->log(msg, level, file, line);
        else
            StandardLog::sLog(msg, level);
    }

    void log_fatal(std::string_view msg) //TODO: Add Fatal as message level
    {
#if WINDOWS
        OutputDebugStringA(msg.data());
        OutputDebugStringA("\n");
#else
//#    error "Unsupported Platform!"
#endif
    }

    LogDummy::LogDummy(MessageType lvl, const char *file, size_t line)
        : mLvl(lvl)
        , mFile(file)
        , mLine(line)
    {
    }

    LogDummy::~LogDummy()
    {
        log(mStream.str(), mLvl, mFile, mLine);
    }

}
}
