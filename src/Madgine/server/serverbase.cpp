#include "../baselib.h"

#include "serverbase.h"

#include "Interfaces/threading/framelistener.h"

#include "../core/root.h"

#include "Interfaces/generic/keyvalueiterate.h"
#include "Interfaces/scripting/types/api.h"



namespace Engine
{
	namespace Server
	{
		ServerBase::ServerBase(const std::string& name) :
			Scope(Scripting::LuaState::getSingleton()),
			mLog(name + "-Log"),
			mName(name)
		{
			Util::setLog(&mLog);			
		}

		ServerBase::~ServerBase()
		{			
			mInstances.clear();
			Util::setLog(nullptr);
		}


		/*int ServerBase::go()
		{					
			start();			

			mLog.startConsole([this](const std::string& cmd) { return performCommand(cmd); });

			int result = FrameLoop::go();

			mLog.stopConsole();

			stop();			

			return result;
		}*/

		ServerLog& ServerBase::log()
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
			return Scope::maps().merge(mInstances, this, MAP_F(shutdown));
		}
	}
}
