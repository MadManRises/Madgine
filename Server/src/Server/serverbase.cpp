#include "serverlib.h"

#include "ServerBase.h"

#include "App\framelistener.h"

#include <iostream>

namespace Engine {
	namespace Server {
		ServerBase::ServerBase(const std::string & name, const std::string &scriptsFolder) :
			mLog(name + "-Log"),
			mName(name),
			mScriptParser(scriptsFolder),
			mRunning(false)
		{
		}
		int ServerBase::run()
		{
			Util::UtilMethods::setup(&mLog);

			mRunning = true;

			start();

			mLog.startConsole(mRunning, [this](const std::string &cmd) {return performCommand(cmd); });
			
			while (update());

			stop();

			mRunning = false;

			return 0;
		}

		Util::StandardLog & Engine::Server::ServerBase::getLog()
		{
			return mLog;
		}

		const std::string & ServerBase::scriptsFolder()
		{
			return mScriptParser.rootFolder();
		}

		void ServerBase::shutdown()
		{
			mRunning = false;
		}

		bool ServerBase::update()
		{
			mConnectionManager.update();
			return mRunning;
		}

		bool ServerBase::performCommand(const std::string & cmd)
		{
			if (cmd == "shutdown") {
				shutdown();
				return true;
			}
			return false;
		}

	}
}
