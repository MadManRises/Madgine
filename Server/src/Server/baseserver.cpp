#include "serverlib.h"

#include "baseserver.h"

namespace Engine {
	namespace Server {
		BaseServer::BaseServer(const std::string & name, const std::string &scriptsFolder) :
			mLog(name + "-Log"),
			mName(name),
			mScriptParser(scriptsFolder),
			mRunning(false)
		{
		}
		int BaseServer::run()
		{
			Util::UtilMethods::setup(&mLog);

			mRunning = true;

			start();

			float timeSinceLastFrame = 0.0f;

			while (mRunning) {
				mRunning = update(timeSinceLastFrame);
				for (auto it = mFrameCallbacks.begin(); it != mFrameCallbacks.end() && mRunning; ++it) {
					mRunning = (*it)(timeSinceLastFrame);
				}
			}

			stop();

			return 0;
		}

		ServerLog & Engine::Server::BaseServer::getLog()
		{
			return mLog;
		}

		const std::string & BaseServer::scriptsFolder()
		{
			return mScriptParser.rootFolder();
		}

		void BaseServer::addFrameCallback(std::function<bool(float)> callback)
		{
			mFrameCallbacks.push_back(callback);
		}

	}
}
