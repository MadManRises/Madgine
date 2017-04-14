#pragma once

#include "Util\standardlog.h"
#include "Scripting/Parsing/serverscriptparser.h"
#include "ServerTimer.h"

#include "serverappinstance.h"

namespace Engine {
	namespace Server {

		class MADGINE_SERVER_EXPORT ServerBase {
		public:
			ServerBase(const std::string &name, const std::string &scriptsFolder);

			int run();

			Util::StandardLog &getLog();

			const std::string &scriptsFolder();		

			void shutdown();

		protected:
			virtual bool update() = 0;
			virtual void start() = 0;
			virtual void stop() = 0;

			template <class T>
			void spawnInstance(T &&init) {
				mInstances.emplace_back(std::forward<T>(init));
			}

		private:
			Util::StandardLog mLog;
			std::string mName;
			Scripting::Parsing::ServerScriptParser mScriptParser;
			ServerTimer mTimer;

			bool mRunning;

			std::list<ServerAppInstance> mInstances;
		};

	}
}