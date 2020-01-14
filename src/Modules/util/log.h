#pragma once

#include "messagetype.h"

namespace Engine {
namespace Util {

    struct MODULES_EXPORT Log {
        virtual ~Log() = default;
        virtual void log(const std::string &msg, MessageType lvl);
        virtual std::string getName() = 0;

        void addListener(LogListener *listener);
        void removeListener(LogListener *listener);

    private:
        std::list<LogListener *> mListeners;
    };

}
}
