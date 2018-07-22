#pragma once

#include "../app/application.h"
#include "../app/appsettings.h"

namespace Engine
{
	namespace Server
	{
		class MADGINE_BASE_EXPORT ServerAppInstance
		{
		public:
			template <class T>
			ServerAppInstance(T&& initCallback, Core::Root &root) :
				mRoot(root),
				mApplication(nullptr),
				mName(std::string("thread_") + std::to_string(++sInstanceCounter)),
				mResult(0),
				mThread(&ServerAppInstance::go<T>, this, std::forward<T>(initCallback))
			{
			}

			virtual ~ServerAppInstance();

			const char* key() const;

			ValueType toValueType() const;

		protected:
			template <class T>
			void go(T initCallback)
			{
				try{
					mResult = -1;

					SignalSlot::ConnectionManager conMgr;

					App::Application app(mRoot);
					mApplication = &app;
					App::AppSettings settings;
					settings.mRunMain = false;
					app.setup(settings);
					if (app.callInit())
					{
						try{
							TupleUnpacker<>::call(initCallback, std::make_tuple(std::ref(app)));
							mResult = app.go();
						}
						catch(...)
						{
							app.callFinalize();	
							throw;
						}
						app.callFinalize();
					}
				}
				catch(const std::exception &e)	{
					LOG_ERROR(Database::message("Uncaught Exception in Thread: ", "")(e.what()));
				}
				catch(...)
				{
					LOG_ERROR("Uncaught unknown Exception in Thread!");
				}
			}

		private:
			Engine::Core::Root &mRoot;
			App::Application* mApplication;

			std::string mName;
			static size_t sInstanceCounter;

			int mResult;
			std::thread mThread;
		};
	}
}
