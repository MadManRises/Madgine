#pragma once

#include "app/serverapplication.h"
#include "scripting/parsing/scriptparser.h"

namespace Engine {

	namespace Server {

		class MADGINE_SERVER_EXPORT ServerAppInstance {
		public:
			template <class T>
			ServerAppInstance(T &&initCallback, const Scripting::LuaTable &table) :				
				mTable(table),
				mApplication(nullptr),
				mName(std::string("thread_") + std::to_string(++sInstanceCounter)),
				mResult(0),
				mThread(&ServerAppInstance::run<T>, this, std::forward<T>(initCallback))				
			{

			}
			virtual ~ServerAppInstance();

			const std::string &key() const;

			ValueType toValueType() const;

		protected:
			template <class T>
			void run(T initCallback) 
			{
				App::ServerApplication app;
				mApplication = &app;
				App::ServerAppSettings settings(mTable);
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
			Scripting::LuaTable mTable;			
			App::ServerApplication *mApplication;

			std::string mName;
			static size_t sInstanceCounter;
			
			int mResult;
			std::thread mThread;
		};

	}

}