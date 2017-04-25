#pragma once

#include "Util\serverlog.h"
#include "Scripting/Parsing/serverscriptparser.h"

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

			virtual bool performCommand(const std::string &cmd);

			template <class T>
			void spawnInstance(T &&init) {
				mInstances.emplace_back(std::forward<T>(init));
			}


		private:
			SignalSlot::ConnectionManager mConnectionManager;

			Util::ServerLog mLog;
			std::string mName;
			Scripting::Parsing::ServerScriptParser mScriptParser;

			bool mRunning;

			std::list<ServerAppInstance> mInstances;

			
		};

	}
}