#include "../interfaceslib.h"

#include "log.h"
#include "loglistener.h"

namespace Engine {
	namespace Util {

		void Log::log(const std::string& msg, MessageType lvl, const std::list<TraceBack>& traceBack) {
			for (LogListener* listener : mListeners)
			{
				listener->messageLogged(msg, lvl, traceBack, getName());
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