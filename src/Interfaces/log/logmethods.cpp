#include "../interfaceslib.h"

#include "log.h"
#include "standardlog.h"

#if WINDOWS
#    define NOMINMAX
#    include <Windows.h>
#endif

namespace Engine {
namespace Log {

    Log *sLog = &StandardLog::getSingleton();

    void setLog(Log *log)
    {
        sLog = log;
    }

    void log(std::string_view msg, MessageType level, const char *file, size_t line)
    {
#if WINDOWS
        if (level == MessageType::FATAL_TYPE || level == MessageType::DEBUG_TYPE) {
            OutputDebugStringA(msg.data());
            OutputDebugStringA("\n");
        }
#endif
        sLog->log(msg, level, file, line);
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
