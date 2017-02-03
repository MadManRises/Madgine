#pragma once

#include "Util/UtilMethods.h"

namespace Engine {
	namespace Util {


		class MADGINE_EXPORT OgreUtilLog : public Log {
		public:
			OgreUtilLog(const std::string &name);
			virtual ~OgreUtilLog();

			// Inherited via Log
			virtual void log(const std::string & msg, MessageType lvl, const std::list<TraceBack>& traceBack = {}) override;


		private:
			Ogre::Log *mLog;
			
		};

	}
}