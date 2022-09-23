#pragma once

namespace Engine {
namespace Util {

    struct INTERFACES_EXPORT Log {
        virtual ~Log() = default;
        virtual void log(std::string_view msg, MessageType lvl, const char *file = nullptr, size_t line = 0);
        virtual std::string getName() = 0;

        void addListener(LogListener *listener);
        void removeListener(LogListener *listener);

    private:
        std::list<LogListener *> mListeners;
    };

}
}
