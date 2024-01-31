#pragma once

#include "messagetype.h"

#include "Generic/tag_invoke.h"

namespace Engine {
namespace Log {

    INTERFACES_EXPORT void setLog(Log *log = nullptr);
    INTERFACES_EXPORT void log(std::string_view msg, MessageType lvl, const char *file = nullptr, size_t line = 0);

    struct INTERFACES_EXPORT LogDummy {
        LogDummy(MessageType lvl, const char *file = nullptr, size_t line = 0);
        ~LogDummy();

        template <typename T>
        LogDummy &operator<<(T &&t)
        {
            mStream << std::forward<T>(t);
            return *this;
        }

    private:
        std::ostringstream mStream;
        MessageType mLvl;
        const char *mFile;
        size_t mLine;
    };

#if ENABLE_DEBUG_LOGGING
#    define LOG_DEBUG(s) Engine::Log::LogDummy { Engine::Log::MessageType::DEBUG_TYPE } << s
#else
#    define LOG_DEBUG(s)
#endif
#define LOG(s) Engine::Log::LogDummy { Engine::Log::MessageType::INFO_TYPE, __FILE__, __LINE__ } << s
#define LOG_WARNING(s) Engine::Log::LogDummy { Engine::Log::MessageType::WARNING_TYPE, __FILE__, __LINE__ } << s
#define LOG_ERROR(s) Engine::Log::LogDummy { Engine::Log::MessageType::ERROR_TYPE, __FILE__, __LINE__ } << s
#define LOG_FATAL(s) Engine::Log::LogDummy { Engine::Log::MessageType::FATAL_TYPE, __FILE__, __LINE__ } << s

#define ONCE(f) std::call_once([]() -> std::once_flag & {static std::once_flag dummy; return dummy; }(), [&]() { f; })

#define LOG_ONCE(s) ONCE(LOG(s))
#define LOG_WARNING_ONCE(s) ONCE(LOG_WARNING(s))
#define LOG_ERROR_ONCE(s) ONCE(LOG_ERROR(s))

    struct log_for_t {
        template <typename T>
        friend auto tag_invoke(log_for_t, MessageType lvl, T &value)
        {
            return LogDummy { lvl };
        }

        template <typename T>
        requires tag_invocable<log_for_t, MessageType, T&>
        auto operator()(MessageType lvl, T &value) const
            noexcept(is_nothrow_tag_invocable_v<log_for_t, MessageType, T&>)
                -> tag_invoke_result_t<log_for_t, MessageType, T&>
        {
            return tag_invoke(*this, lvl, value);
        }

    };

    inline constexpr log_for_t log_for;

}
}
