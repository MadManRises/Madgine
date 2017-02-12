#pragma once

#include "Util\UtilMethods.h"

namespace Engine {
	namespace Server {

		class MADGINE_SERVER_EXPORT ServerLog : public ::Util::Log {
		public:
			ServerLog(const std::string &name);

			// Inherited via Log
			virtual void log(const std::string & msg, ::Util::MessageType lvl, const std::list<::Util::TraceBack>& traceBack = {}) override;
			virtual std::string getName() override;

		private:
			std::string mName;
		};

	}
}