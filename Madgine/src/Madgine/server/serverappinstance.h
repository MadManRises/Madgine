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
			ServerAppInstance(T&& initCallback, Engine::App::Root &root) :
				mRoot(root),
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

					SignalSlot::ConnectionManager conMgr;

					App::ServerApplication app(mRoot);
					mApplication = &app;
					App::ServerAppSettings settings;
					app.setup(settings);
					if (app.init())
					{
						try{
							TupleUnpacker<>::call(initCallback, std::make_tuple(std::ref(app)));
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
			Engine::App::Root &mRoot;
			App::ServerApplication* mApplication;

			std::string mName;
			static size_t sInstanceCounter;

			int mResult;
			std::thread mThread;
		};
	}
}
