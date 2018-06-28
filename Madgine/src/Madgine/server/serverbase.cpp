#include "../serverlib.h"

#include "serverbase.h"

#include "../app/framelistener.h"

#include "../app/root.h"

API_IMPL(Engine::Server::ServerBase, MAP_F(shutdown));

namespace Engine
{
	namespace Server
	{
		ServerBase::ServerBase(const std::string& name, App::Root &root) :
			Scope(root.luaState()),
			mLog(name + "-Log"),
			mName(name),
			mRunning(false),
			mRoot(root)
		{
			Util::UtilMethods::setup(&mLog);
		}

		ServerBase::~ServerBase()
		{
			mInstances.clear();
			Util::UtilMethods::setup(nullptr);
		}


		int ServerBase::run()
		{		

			init();

			mRunning = true;

			start();

			mLog.startConsole(mRunning, [this](const std::string& cmd) { return performCommand(cmd); });

			while (sendFrameStarted() && frame() && sendFrameEnded()) std::this_thread::yield();

			stop();

			mRunning = false;

			finalize();

			return 0;
		}

		Util::ServerLog& ServerBase::log()
		{
			return mLog;
		}

		void ServerBase::shutdown()
		{
			mRunning = false;
		}

		bool ServerBase::frame()
		{
			SignalSlot::ConnectionManager::getSingleton().update();
			return mRunning;
		}

		bool ServerBase::performCommand(const std::string& cmd)
		{
			if (cmd == "shutdown")
			{
				shutdown();
				return true;
			}
			return false;
		}

		void ServerBase::addFrameListener(App::FrameListener* listener)
		{
			mListeners.push_back(listener);
		}

		void ServerBase::removeFrameListener(App::FrameListener* listener)
		{
			mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
		}

		bool ServerBase::sendFrameStarted()
		{
			bool result = true;
			for (App::FrameListener* listener : mListeners)
				result &= listener->frameStarted(0);
			if (!result)
				return false;
			for (App::FrameListener* listener : mListeners)
				result &= listener->frameRenderingQueued(0);
			return result;
		}

		bool ServerBase::sendFrameEnded()
		{
			bool result = true;
			for (App::FrameListener* listener : mListeners)
				result &= listener->frameEnded(0);
			return result;
		}

		KeyValueMapList ServerBase::maps()
		{
			return Scope::maps().merge(mInstances);
		}
	}
}
