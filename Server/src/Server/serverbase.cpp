#include "serverlib.h"

#include "ServerBase.h"

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

			mTimer.reset();

			while (mRunning) {
				float timeSinceLastFrame = mTimer.elapsed_ns() / 1000000000.0f;
				mTimer.start();
				mRunning = update(timeSinceLastFrame);
				for (auto it = mFrameCallbacks.begin(); it != mFrameCallbacks.end() && mRunning; ++it) {
					mRunning = (*it)(timeSinceLastFrame);
				}
			}

			stop();

			return 0;
		}

		ServerLog & Engine::Server::ServerBase::getLog()
		{
			return mLog;
		}

		const std::string & ServerBase::scriptsFolder()
		{
			return mScriptParser.rootFolder();
		}

		void ServerBase::addFrameCallback(std::function<bool(float)> callback)
		{
			mFrameCallbacks.push_back(callback);
		}

	}
}
