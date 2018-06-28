#pragma once

#include "../util/serverlog.h"
#include "../scripting/types/globalscopebase.h"
#include "../scripting/types/luastate.h"

#include "serverappinstance.h"


namespace Engine
{
	namespace Server
	{
		class MADGINE_SERVER_EXPORT ServerBase : public Scripting::Scope<ServerBase, Scripting::GlobalScopeBase>,
		                                         public MadgineObject
		{
		public:
			ServerBase(const std::string& name, App::Root &root);
			virtual ~ServerBase();

			int run();

			Util::ServerLog& log();

			void shutdown();

			void addFrameListener(App::FrameListener* listener);
			void removeFrameListener(App::FrameListener* listener);

		protected:
			virtual bool frame() = 0;
			virtual void start() = 0;
			virtual void stop() = 0;

			virtual bool performCommand(const std::string& cmd);

			template <class T>
			void spawnInstance(T&& init)
			{
				mInstances.emplace_back(std::forward<T>(init), mRoot);
			}

			bool sendFrameStarted();
			bool sendFrameEnded();

			KeyValueMapList maps() override;

		private:

			Util::ServerLog mLog;
			std::string mName;

			App::Root &mRoot;

			bool mRunning;

			std::list<ServerAppInstance> mInstances;

			std::vector<App::FrameListener*> mListeners;
		};

#define SERVER_INSTANCE(Class) \
		extern "C" DLL_EXPORT Engine::Server::ServerBase *createServer(const std::string &name, Engine::App::Root &root) {\
			return new TW::Server::Server(name, root);\
		}

	}
}
