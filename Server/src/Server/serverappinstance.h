#pragma once

#include "App\serverapplication.h"


namespace Engine {
	namespace Server {

		class MADGINE_SERVER_EXPORT ServerAppInstance {
		public:
			template <class T>
			ServerAppInstance(T &&initCallback) :
				mApplication(nullptr),
				mResult(0),
				mThread(&ServerAppInstance::run<T>, this, std::forward<T>(initCallback))
			{

			}
			virtual ~ServerAppInstance();

		protected:
			template <class T>
			void run(T initCallback) 
			{
				App::ServerApplication app;
				mApplication = &app;
				App::ServerAppSettings settings;
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