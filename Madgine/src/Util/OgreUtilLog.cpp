
#include "madginelib.h"

#include "OgreUtilLog.h"

namespace Engine {
	namespace Util {		



		OgreUtilLog::OgreUtilLog(const std::string & name)
		{
			mLog = Ogre::LogManager::getSingleton().createLog(name);
			mLog->setLogDetail(Ogre::LL_BOREME);
		}

		OgreUtilLog::~OgreUtilLog()
		{
			Ogre::LogManager::getSingleton().destroyLog(mLog);
		}

		void OgreUtilLog::log(const std::string & msg, MessageType lvl, const std::list<TraceBack>& traceBack)
		{
			Ogre::LogMessageLevel level;
			switch (lvl) {
			case ERROR_TYPE:
				level = Ogre::LML_CRITICAL;
				break;
			case WARNING_TYPE:
				level = Ogre::LML_NORMAL;
				break;
			case LOG_TYPE:
				level = Ogre::LML_TRIVIAL;
				break;
			default:
				throw 0;
			}
			mLog->logMessage(msg.c_str(), level);
		}

	}
}