#include "../interfaceslib.h"

#include "log.h"
#include "loglistener.h"

namespace Engine {
namespace Log {

    void Log::log(std::string_view msg, MessageType lvl, const char *file, size_t line)
    {
        for (LogListener *listener : mListeners) {
            listener->messageLogged(msg, lvl, file, line, this);
        }
    }

    void Log::addListener(LogListener *listener)
    {
        mListeners.push_back(listener);
    }

    void Log::removeListener(LogListener *listener)
    {
        mListeners.remove(listener);
    }

}
}