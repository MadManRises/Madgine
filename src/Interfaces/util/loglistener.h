#pragma once

#include "../debug/stacktrace.h"

namespace Engine {
namespace Util {

    struct LogListener {
        virtual ~LogListener() = default;
        virtual void messageLogged(std::string_view message, MessageType lml, const char *file, Log *log) = 0;
    };

}
}
