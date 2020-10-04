#pragma once

#include "messagetype.h"

namespace Engine {
namespace Util {

    INTERFACES_EXPORT void setLog(Log *log = nullptr);
    INTERFACES_EXPORT void log(const std::string &msg, MessageType lvl);

    struct INTERFACES_EXPORT LogDummy {
        LogDummy(MessageType lvl);
        ~LogDummy();

        template <typename T>
        LogDummy &operator<<(T &&t)
        {
            mStream << std::forward<T>(t);
            return *this;
        }

    private:
        std::stringstream mStream;
        MessageType mLvl;
    };

#define LOG(s) Engine::Util::LogDummy(Engine::Util::LOG_TYPE) << s
#define LOG_WARNING(s) Engine::Util::LogDummy(Engine::Util::WARNING_TYPE) << s
#define LOG_ERROR(s) Engine::Util::LogDummy(Engine::Util::ERROR_TYPE) << s
#define LOG_EXCEPTION(e) LOG_ERROR(e.what())

#define LOG_ONCE(s) std::call_once([]() -> std::once_flag & {static std::once_flag dummy; return dummy; }(), []() { LOG(s); })
#define LOG_WARNING_ONCE(s) std::call_once([]() -> std::once_flag & {static std::once_flag dummy; return dummy; }(), []() { LOG_WARNING(s); })
#define LOG_ERROR_ONCE(s) std::call_once([]() -> std::once_flag & {static std::once_flag dummy; return dummy; }(), []() { LOG_ERROR(s); })

}
}
