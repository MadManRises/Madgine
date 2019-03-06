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
		class MADGINE_BASE_EXPORT ServerBase : public Scripting::Scope<ServerBase, Scripting::GlobalScopeBase>, public Threading::FrameLoop
		{
		public:
			ServerBase(const std::string& name);
			virtual ~ServerBase();

			ServerLog& log();		

		protected:
			virtual void start() = 0;
			virtual void stop() = 0;

			virtual bool performCommand(const std::string& cmd);

			template <class T>
			void spawnInstance(T&& init)
			{
				mInstances.emplace_back(std::forward<T>(init));
			}

			KeyValueMapList maps() override;

		private:

			ServerLog mLog;
			std::string mName;

			std::list<ServerAppInstance> mInstances;
			
		};

#define SERVER_INSTANCE(Class) \
		extern "C" DLL_EXPORT Engine::Server::ServerBase *createServer(const std::string &name) {\
			return new TW::Server::Server(name);\
		}

	}
}

RegisterType(Engine::Server::ServerBase);