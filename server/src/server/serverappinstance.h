#pragma once

#include "app/serverapplication.h"


namespace Engine {
	namespace Server {

		class MADGINE_SERVER_EXPORT ServerAppInstance {
		public:
			template <class T>
			ServerAppInstance(T &&initCallback, lua_State *state) :
				mApplication(nullptr),
				mResult(0),
				mThread(&ServerAppInstance::run<T>, this, std::forward<T>(initCallback), state)
			{

			}
			virtual ~ServerAppInstance();

		protected:
			template <class T>
			void run(T initCallback, lua_State *state) 
			{
				App::ServerApplication app;
				mApplication = &app;
				App::ServerAppSettings settings(state);
				app.setup(settings);
				if (app.init()) {
					initCallback();
					mResult = app.go();
				}
				else {
					mResult = -1;
				}
			}

		private:
			App::ServerApplication *mApplication;
			int mResult;
			
			std::thread mThread;
			
		};

	}
}