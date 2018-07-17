#include "../baselib.h"

#include "serverbase.h"

#include "../core/framelistener.h"

#include "../core/root.h"

API_IMPL(Engine::Server::ServerBase, MAP_F(shutdown));

namespace Engine
{
	namespace Server
	{
		ServerBase::ServerBase(const std::string& name, Core::Root &root) :
			Scope(root.luaState()),
			mLog(name + "-Log"),
			mName(name),
			mRunning(false),
			mRoot(root)
		{
			Util::UtilMethods::setup(&mLog);
			addFrameListener(this);
		}

		ServerBase::~ServerBase()
		{
			removeFrameListener(this);
			mInstances.clear();
			Util::UtilMethods::setup(nullptr);
		}


		int ServerBase::go()
		{		

			MadgineObject::init();
			FrameLoop::init();

			mRunning = true;

			start();

			mLog.startConsole(mRunning, [this](const std::string& cmd) { return performCommand(cmd); });

			int result = FrameLoop::go();

			stop();

			mRunning = false;

			FrameLoop::finalize();
			MadgineObject::finalize();

			return result;
		}

		Util::ServerLog& ServerBase::log()
		{
			return mLog;
		}

		void ServerBase::shutdown()
		{
			mRunning = false;
		}

		bool ServerBase::frameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context)
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

		KeyValueMapList ServerBase::maps()
		{
			return Scope::maps().merge(mInstances);
		}
	}
}
