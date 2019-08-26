#pragma once

#if ENABLE_THREADING

#include "serverlog.h"
#include "Modules/keyvalue/scopebase.h"

#include "serverappinstance.h"
#include "Modules/signalslot/taskqueue.h"


namespace Engine
{
	namespace Server
	{
    class MADGINE_SERVER_EXPORT ServerBase : public ScopeBase, public SignalSlot::TaskQueue
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

		private:
			ServerLog mLog;

			std::list<ServerAppInstance> mInstances;	

			std::chrono::steady_clock::time_point mLastConsoleCheck;
		};

	}
}

#endif