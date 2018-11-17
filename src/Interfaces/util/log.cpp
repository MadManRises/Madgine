#include "../interfaceslib.h"

#include "log.h"
#include "loglistener.h"

namespace Engine {
	namespace Util {

		void Log::log(const std::string& msg, MessageType lvl) {
			for (LogListener* listener : mListeners)
			{
				listener->messageLogged(msg, lvl, Debug::StackTrace<32>::getCurrent(3), getName());
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