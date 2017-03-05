
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

		void OgreUtilLog::log(const std::string & msg, ::Util::MessageType lvl, const std::list<::Util::TraceBack>& traceBack)
		{
			Ogre::LogMessageLevel level;
			switch (lvl) {
			case ::Util::ERROR_TYPE:
				level = Ogre::LML_CRITICAL;
				break;
			case ::Util::WARNING_TYPE:
				level = Ogre::LML_NORMAL;
				break;
			case ::Util::LOG_TYPE:
				level = Ogre::LML_TRIVIAL;
				break;
			default:
				throw 0;
			}
			mLog->logMessage(msg.c_str(), level);
		}

		std::string OgreUtilLog::getName()
		{
			return mLog->getName();
		}

	}
}