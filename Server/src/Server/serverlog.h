#pragma once

#include "Util\UtilMethods.h"

namespace Engine {
	namespace Server {

		class MADGINE_SERVER_EXPORT ServerLog : public Engine::Util::Log {
		public:
			ServerLog(const std::string &name);

			// Inherited via Log
			virtual void log(const std::string & msg, Engine::Util::MessageType lvl, const std::list<Engine::Util::TraceBack>& traceBack = {}) override;
			virtual std::string getName() override;

		private:
			std::string mName;
		};

	}
}