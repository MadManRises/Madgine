#pragma once

#include "../app/serverapplication.h"
#include "../app/serverappsettings.h"

namespace Engine
{
	namespace Server
	{
		class MADGINE_SERVER_EXPORT ServerAppInstance
		{
		public:
			template <class T>
			ServerAppInstance(T&& initCallback, Scripting::LuaState *state) :
				mState(state),
				mApplication(nullptr),
				mName(std::string("thread_") + std::to_string(++sInstanceCounter)),
				mResult(0),
				mThread(&ServerAppInstance::run<T>, this, std::forward<T>(initCallback))
			{
			}

			virtual ~ServerAppInstance();

			const char* key() const;

			ValueType toValueType() const;

		protected:
			template <class T>
			void run(T initCallback)
			{
				try{
					mResult = -1;

					App::ServerApplication app(mState);
					mApplication = &app;
					App::ServerAppSettings settings;
					app.setup(settings);
					if (app.init())
					{
						try{
							initCallback();
							mResult = app.go();
						}
						catch(...)
						{
							app.finalize();	
							throw;
						}
						app.finalize();
					}
				}
				catch(const std::exception &e)	{
					LOG_ERROR(message("Uncaught Exception in Thread: ", "")(e.what()));
				}
				catch(...)
				{
					LOG_ERROR("Uncaught unknown Exception in Thread!");
				}
			}

		private:
			Scripting::LuaState *mState;
			App::ServerApplication* mApplication;

			std::string mName;
			static size_t sInstanceCounter;

			int mResult;
			std::thread mThread;
		};
	}
}
