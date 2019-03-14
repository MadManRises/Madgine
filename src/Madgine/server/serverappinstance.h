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
				mWorkGroup(&ServerAppInstance::go<T>, this, std::forward<T>(initCallback))
			{
			}

			virtual ~ServerAppInstance();

			const char* key() const;

			ValueType toValueType() const;

		protected:
			template <class T>
			int go(T initCallback, Threading::WorkGroup &workgroup)
			{
				//TODO
				throw 0;
				App::AppSettings settings;
				settings.mRunMain = false;
				App::Application app(settings);				
				mApplication = &app;
				return Threading::Scheduler(workgroup, { &app.frameLoop() }).go();
			}

		private:
			App::Application* mApplication;

			std::string mName;
			static size_t sInstanceCounter;

			Threading::WorkGroupHandle mWorkGroup;
		};
	}
}
