#include "../interfaceslib.h"

#include "log.h"
#include "loglistener.h"

namespace Engine {
	namespace Util {

		void Log::log(std::string_view msg, MessageType lvl)
            {
			for (LogListener* listener : mListeners)
			{
				listener->messageLogged(msg, lvl, Debug::StackTrace<32>::getCurrent(3), this);
			}
		}

		void Log::addListener(LogListener *listener) {
			mListeners.push_back(listener);
		}

		void Log::removeListener(LogListener *listener) {
			mListeners.remove(listener);
		}

	}
}