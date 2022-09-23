#pragma once

#include "Interfaces/util/standardlog.h"

namespace Engine {
namespace Server {
    struct MADGINE_SERVER_EXPORT ServerLog : Util::StandardLog {
        ServerLog(const std::string &name);
        ~ServerLog();

        void log(std::string_view msg, Util::MessageType lvl, const char *file, size_t line) override;

        bool startConsole();
        void stopConsole();

        std::vector<std::string> update();

    private:
        bool mConsole;
        std::string mCurrentCmd;
    };
}
}
