#pragma once

#include "../util/serverlog.h"
#include "../scripting/types/globalscopebase.h"
#include "../scripting/types/luastate.h"

#include "serverappinstance.h"
#include "../core/frameloop.h"
#include "../core/framelistener.h"


namespace Engine
{
	namespace Server
	{
		class MADGINE_BASE_EXPORT ServerBase : public Scripting::Scope<ServerBase, Scripting::GlobalScopeBase>,
			public Core::FrameListener, public Core::FrameLoop
		{
		public:
			ServerBase(const std::string& name, Core::Root &root);
			virtual ~ServerBase();

			virtual int go() override;

			Util::ServerLog& log();

			void shutdown();			

		protected:
			virtual bool frameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context) override = 0;
			virtual void start() = 0;
			virtual void stop() = 0;

			virtual bool performCommand(const std::string& cmd);

			template <class T>
			void spawnInstance(T&& init)
			{
				mInstances.emplace_back(std::forward<T>(init), mRoot);
			}

			KeyValueMapList maps() override;

		private:

			Util::ServerLog mLog;
			std::string mName;

			Core::Root &mRoot;

			bool mRunning;

			std::list<ServerAppInstance> mInstances;
			
		};

#define SERVER_INSTANCE(Class) \
		extern "C" DLL_EXPORT Engine::Server::ServerBase *createServer(const std::string &name, Engine::Core::Root &root) {\
			return new TW::Server::Server(name, root);\
		}

	}
}
