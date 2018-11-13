
#include "../maditorlauncherlib.h"

#include "launcherloglistener.h"


namespace Maditor {
	namespace Launcher {

		LauncherLogListener::LauncherLogListener()
		{

		}

		LauncherLogListener::~LauncherLogListener() {

		}

		void LauncherLogListener::init() {
			mSlot = std::make_unique<SlotType>(&receiveMessage);
		}

		void LauncherLogListener::receiveImpl(const std::string & msg, Engine::Util::MessageType level, const std::string & logName, const std::string & fullTraceback, const std::string & fileName, int lineNr)
		{
		}



		void LauncherLogListener::messageLogged(const std::string & message, Engine::Util::MessageType lml, const Engine::Debug::StackTrace<32> &stackTrace, const std::string & logName)
		{
			std::stringstream fullTraceback;
			Engine::Debug::FullStackTrace full = stackTrace.calculateReadable();
			for (const auto &trace : full) {
				fullTraceback << trace << '\n';
			}

			const Engine::Debug::TraceBack &t = full.front();

			mSlot->queue(message, lml, logName, fullTraceback.str(), t.mFile, t.mLineNr, {});
		}

	}
}
