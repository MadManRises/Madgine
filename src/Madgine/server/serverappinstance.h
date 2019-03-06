#pragma once

#include "../app/application.h"
#include "../app/appsettings.h"
#include "Interfaces/threading/workgrouphandle.h"

namespace Engine
{
	namespace Server
	{
		class MADGINE_BASE_EXPORT ServerAppInstance
		{
		public:
			template <class T>
			ServerAppInstance(T&& initCallback) :
				mApplication(nullptr),
				mName("thread_"s + std::to_string(++sInstanceCounter)),
				mResult(0),
				mWorkGroup(&ServerAppInstance::go<T>, this, std::forward<T>(initCallback))
			{
			}

			virtual ~ServerAppInstance();

			const char* key() const;

			ValueType toValueType() const;

		protected:
			template <class T>
			void go(T initCallback, Threading::WorkGroup &workgroup)
			{
				mResult = -1;

				App::AppSettings settings;
				settings.mRunMain = false;
				App::Application app(settings, workgroup);				
				mApplication = &app;
				if (app.callInit())
				{
					try {
						TupleUnpacker::invoke(initCallback, app);
						mResult = Threading::Scheduler(workgroup, { &app.frameLoop() }).go();
					}
					catch (...)
					{
						app.callFinalize();
						throw;
					}
					app.callFinalize();
				}
			}

		private:
			App::Application* mApplication;

			std::string mName;
			static size_t sInstanceCounter;

			int mResult;
			Threading::WorkGroupHandle mWorkGroup;
		};
	}
}
