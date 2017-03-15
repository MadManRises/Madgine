#pragma once

#include "Util/UtilMethods.h"

namespace Engine {
	namespace Util {


		class OGREMADGINE_EXPORT OgreUtilLog : public Engine::Util::Log {
		public:
			OgreUtilLog(const std::string &name);
			virtual ~OgreUtilLog();

			// Inherited via Log
			virtual void log(const std::string & msg, Engine::Util::MessageType lvl, const std::list<Engine::Util::TraceBack>& traceBack = {}) override;
			virtual std::string getName() override;

		private:
			Ogre::Log *mLog;
			
		};

	}
}