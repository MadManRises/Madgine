#pragma once

#include "Util/UtilMethods.h"

namespace Engine {
	namespace Util {


		class OGREMADGINE_EXPORT OgreUtilLog : public ::Util::Log {
		public:
			OgreUtilLog(const std::string &name);
			virtual ~OgreUtilLog();

			// Inherited via Log
			virtual void log(const std::string & msg, ::Util::MessageType lvl, const std::list<::Util::TraceBack>& traceBack = {}) override;
			virtual std::string getName() override;

		private:
			Ogre::Log *mLog;
			
		};

	}
}