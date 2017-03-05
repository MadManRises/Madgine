#pragma once

#include "serverlog.h"
#include "Scripting/Parsing/serverscriptparser.h"
#include "ServerTimer.h"

#include "serverappinstance.h"

namespace Engine {
	namespace Server {

		class MADGINE_SERVER_EXPORT BaseServer {
		public:
			BaseServer(const std::string &name, const std::string &scriptsFolder);

			int run();

			ServerLog &getLog();

			const std::string &scriptsFolder();

			void addFrameCallback(std::function<bool(float)> callback);

		protected:
			virtual bool update(float timeSinceLastFrame) = 0;
			virtual void start() = 0;
			virtual void stop() = 0;

			template <class T>
			void spawnInstance(T &&init) {
				mInstances.emplace_back(std::forward<T>(init));
			}

		private:
			ServerLog mLog;
			std::string mName;
			Scripting::Parsing::ServerScriptParser mScriptParser;
			ServerTimer mTimer;

			bool mRunning;

			std::list<std::function<bool(float)>> mFrameCallbacks;

			std::list<ServerAppInstance> mInstances;
		};

	}
}