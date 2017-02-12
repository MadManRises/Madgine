#include "serverlib.h"
#include "serverlog.h"

#include <iostream>

namespace Engine {
	namespace Server {

		ServerLog::ServerLog(const std::string & name) :
			mName(name)
		{
		}

		void ServerLog::log(const std::string & msg, Util::MessageType lvl, const std::list<Util::TraceBack>& traceBack)
		{
			std::cout << msg;
		}

		std::string ServerLog::getName()
		{
			return mName;
		}

	}
}


