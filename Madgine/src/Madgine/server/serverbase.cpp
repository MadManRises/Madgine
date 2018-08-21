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
			mRoot(root)
		{
			Util::UtilMethods::setup(&mLog);			
		}

		ServerBase::~ServerBase()
		{			
			mInstances.clear();
			Util::UtilMethods::setup(nullptr);
		}


		int ServerBase::go()
		{					
			FrameLoop::init();

			start();			

			mLog.startConsole([this](const std::string& cmd) { return performCommand(cmd); });

			int result = FrameLoop::go();

			mLog.stopConsole();

			stop();

			FrameLoop::finalize();

			return result;
		}

		Util::ServerLog& ServerBase::log()
		{
			return mLog;
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
			return Scope::maps().merge(mInstances, this);
		}
	}
}
