#pragma once

#include "serverlog.h"
#include "Interfaces/scripting/types/globalscopebase.h"
#include "Interfaces/scripting/types/luastate.h"

#include "serverappinstance.h"
#include "Interfaces/threading/frameloop.h"
#include "Interfaces/threading/framelistener.h"


namespace Engine
{
	namespace Server
	{
		class MADGINE_BASE_EXPORT ServerBase : public Scripting::Scope<ServerBase, Scripting::GlobalScopeBase>, public SignalSlot::TaskQueue
		{
		public:
			ServerBase(Threading::WorkGroup &workgroup);
			virtual ~ServerBase();

			ServerLog& log();		

			void shutdown();

		protected:

			virtual void performCommand(const std::string& cmd);

			template <class T>
			void spawnInstance(T&& init, const App::AppSettings &settings = {})
			{
				mInstances.emplace_back(std::forward<T>(init), settings);
			}

			void consoleCheck();

			KeyValueMapList maps() override;

		private:
			ServerLog mLog;

			std::list<ServerAppInstance> mInstances;	

			std::chrono::steady_clock::time_point mLastConsoleCheck;
		};

	}
}

RegisterType(Engine::Server::ServerBase);