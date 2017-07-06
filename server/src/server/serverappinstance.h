#pragma once

#include "app/serverapplication.h"
#include "scripting/parsing/scriptparser.h"

namespace Engine {

	namespace Server {

		class MADGINE_SERVER_EXPORT ServerAppInstance {
		public:
			template <class T>
			ServerAppInstance(T &&initCallback, Scripting::Parsing::ScriptParser &parser) :
				mParser(parser),
				mState(parser.createThread()),
				mApplication(nullptr),
				mName(std::string("thread_") + std::to_string(++sInstanceCounter)),
				mResult(0)/*,
				mThread(&ServerAppInstance::run<T>, this, std::forward<T>(initCallback))				*/
			{

			}
			virtual ~ServerAppInstance();

			const std::string &key() const;
			int push(lua_State *state) const;

		protected:
			template <class T>
			void run(T initCallback) 
			{
				App::ServerApplication app;
				mApplication = &app;
				App::ServerAppSettings settings(mState);
				app.setup(settings);
				if (app.init()) {
					initCallback();
					mResult = app.go();
					app.finalize();
				}
				else {
					mResult = -1;
				}
			}

		private:
			Scripting::Parsing::ScriptParser &mParser;
			std::pair<lua_State *, int> mState;			
			App::ServerApplication *mApplication;

			std::string mName;
			static size_t sInstanceCounter;
			
			int mResult;
			std::thread mThread;
		};

	}

	namespace Scripting {
		template <>
		struct MADGINE_SERVER_EXPORT LuaHelper<Server::ServerAppInstance> {
			static int push(lua_State *state, const Server::ServerAppInstance &it);
		};
	}

}