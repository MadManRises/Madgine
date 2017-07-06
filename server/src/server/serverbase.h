#pragma once

#include "util/serverlog.h"
#include "scripting/parsing/serverscriptparser.h"
#include "scripting/types/globalscopebase.h"

#include "serverappinstance.h"

namespace Engine {
	namespace Server {

		class MADGINE_SERVER_EXPORT ServerBase : public Scripting::Scope<ServerBase, Scripting::GlobalAPIComponentBase> {
		public:
			ServerBase(const std::string &name, const std::string &scriptsFolder);

			int run();

			Util::StandardLog &getLog();

			const std::string &scriptsFolder();		

			void shutdown();


			void addFrameListener(App::FrameListener *listener);
			void removeFrameListener(App::FrameListener *listener);

			Scripting::GlobalScopeBase *globalScope();

		protected:
			virtual bool frame() = 0;
			virtual void start() = 0;
			virtual void stop() = 0;

			virtual bool performCommand(const std::string &cmd);

			template <class T>
			void spawnInstance(T &&init) {
				mInstances.emplace_back(std::forward<T>(init), mScriptParser);
			}

			bool sendFrameStarted();
			bool sendFrameEnded();

			virtual Scripting::KeyValueMapList maps() override;

		private:
			SignalSlot::ConnectionManager mConnectionManager;

			Util::ServerLog mLog;
			std::string mName;
			Scripting::Parsing::ServerScriptParser mScriptParser;
			Scripting::GlobalScopeBase mGlobalScope;

			bool mRunning;

			std::list<ServerAppInstance> mInstances;

			std::list<App::FrameListener*> mListeners;

		};

	}
}