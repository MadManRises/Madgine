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
		ServerBase::ServerBase(Threading::WorkGroup &workgroup) :
			Scope(Scripting::LuaState::getSingleton()),
			mLog(workgroup.name() + "-Log"),
			TaskQueue("Default")
		{
			Util::setLog(&mLog);	
			workgroup.addThreadInitializer([&]() {Util::setLog(&mLog); });
			mLog.startConsole();
			addRepeatedTask([this]() {consoleCheck(); }, std::chrono::milliseconds(20));
		}

		ServerBase::~ServerBase()
		{	
			mLog.stopConsole();
			mInstances.clear();
			Util::setLog(nullptr);			
		}

		ServerLog& ServerBase::log()
		{
			return mLog;
		}

		void ServerBase::shutdown()
		{
			stop();
		}

		void ServerBase::performCommand(const std::string& cmd)
		{
			if (cmd == "shutdown")
			{
				shutdown();				
			}
			else
			{
				LOG("Unknown Command \"" << cmd << "\"!");
			}
		}

		void ServerBase::consoleCheck()
		{
			for (const std::string &cmd : mLog.update())
			{
				performCommand(cmd);
			}
			mLastConsoleCheck = std::chrono::steady_clock::now();
		}

		KeyValueMapList ServerBase::maps()
		{
			return Scope::maps().merge(this, MAP_F(shutdown));
		}

	}
}
