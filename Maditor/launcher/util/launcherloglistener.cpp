
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

#ifdef MADGINE_CLIENT_BUILD
		void LauncherLogListener::messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage)
		{
			if (lml != Ogre::LML_CRITICAL) return;
			Engine::Util::MessageType level;
			switch (lml) {
			case Ogre::LML_CRITICAL:
				level = Engine::Util::ERROR_TYPE;
				break;
			case Ogre::LML_NORMAL:
				level = Engine::Util::LOG_TYPE;
				break;
			case Ogre::LML_TRIVIAL:
				level = Engine::Util::LOG_TYPE;
				break;
			default:
				throw 0;
			}
			mSlot->queue(message.c_str(), level, logName.c_str(), "", "", -1, {});
		}
#endif

		void LauncherLogListener::messageLogged(const std::string & message, Engine::Util::MessageType lml, const std::list<Engine::Util::TraceBack> &traceback, const std::string & logName)
		{
			std::stringstream fullTraceback;
			std::string fileName;
			int lineNr = -2;
			
			const Engine::Util::TraceBack *last = 0;
			for (const Engine::Util::TraceBack &t : Engine::Util::UtilMethods::traceBack()) {
				if (last)
					fullTraceback << std::endl;
				last = &t;
				fullTraceback << /*relative*/t.mFile << "(" << t.mLineNr << "): " << t.mFunction;
			}
			if (last) {
				fileName = last->mFile;
				lineNr = last->mLineNr;
			}
			else {
				lineNr = -1;
			}

			mSlot->queue(message, lml, logName, fullTraceback.str(), fileName, lineNr, {});
		}

	}
}
