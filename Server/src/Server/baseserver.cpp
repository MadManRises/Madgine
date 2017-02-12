#include "serverlib.h"

#include "baseserver.h"

namespace Engine {
	namespace Server {
		BaseServer::BaseServer(const std::string & name) :
			mLog(name + "-Log"),
			mName(name)
		{
		}
		int BaseServer::start()
		{
			Util::UtilMethods::setup(&mLog);

			return run();
		}

		ServerLog & Engine::Server::BaseServer::getLog()
		{
			return mLog;
		}

	}
}
