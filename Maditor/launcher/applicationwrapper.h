#pragma once

#include "../shared/appcontrol.h"

#include "moduleloader.h"
#include "util/launcherloglistener.h"
#include "Madgine/serialize/container/serialized.h"

#include "Madgine/core/framelistener.h"

#include "inspector.h"

#include "../shared/applicationinfo.h"
#include "Madgine/core/root.h"


#include "Madgine/app/clientappsettings.h"
#include "Madgine/app/clientapplication.h"


namespace Maditor {

	

	namespace Launcher {

		class ApplicationWrapper : public Engine::Serialize::SerializableUnit<ApplicationWrapper,Shared::AppControl>, public Engine::Core::FrameListener {

		public:
			ApplicationWrapper();
			~ApplicationWrapper();

			int run();
			
		protected:
			virtual int acceptConnection() = 0;
			virtual Engine::Serialize::SerializeManager *mgr() = 0;
			virtual Engine::Input::InputHandler *input() = 0;


			// Inherited via AppControl
			virtual void shutdownImpl() override;
		
			// Inherited via AppControl
			virtual void onApplicationConnected() override;

			virtual bool frameStarted(std::chrono::microseconds timeSinceLastFrame) override;
			virtual bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Engine::Scene::ContextMask context) override;
			virtual bool frameEnded(std::chrono::microseconds timeSinceLastFrame) override;

			void startImpl();

			void stopImpl();

			void pauseImpl();

			virtual void resizeWindowImpl() override;

			virtual void execLuaImpl(const std::string &cmd) override;

		protected:
			void configImpl(const Shared::ApplicationInfo&) override;

			static int lua_log(lua_State * state);

		private:
			lua_State *mCurrentExecScope;
			std::stringstream mExecOutBuffer;

			static ApplicationWrapper *sInstance;

			std::unique_ptr<Engine::Core::Root> mRoot;

			Engine::Serialize::Serialized<LauncherLogListener> mLog;
			Engine::Serialize::Serialized<ModuleLoader> mLoader;
			Engine::Serialize::Serialized<Inspector> mInspector;

			Shared::ApplicationInfo mAppInfo;
			bool mHaveAppInfo;

			Engine::Core::RootSettings mRootSettings;

			Engine::App::ClientAppSettings mSettings;
			std::unique_ptr<Engine::App::ClientApplication> mApplication;

			std::unique_ptr<Engine::Server::ServerBase> mServer;


			bool mRunning;			
			bool mStartRequested;
			
			
		};

	}
}
