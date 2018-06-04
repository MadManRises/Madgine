#pragma once

#include "shared/appcontrol.h"

#ifdef MADGINE_CLIENT_BUILD
#include "Madgine/app/ogreappsettings.h"
#include "Madgine/app/ogreapplication.h"
#endif
#ifdef MADGINE_SERVER_BUILD
#include "Madgine/app/serverappsettings.h"
#endif

#include "moduleloader.h"
#include "util/launcherloglistener.h"
#include "Madgine/serialize/container/serialized.h"

#include "util/util.h"

#include "Madgine/app/framelistener.h"

#include "inspector.h"

#include "shared/applicationinfo.h"

namespace Maditor {

	

	namespace Launcher {

		class ApplicationWrapper : public Engine::Serialize::SerializableUnit<ApplicationWrapper,Shared::AppControl>, public Engine::App::FrameListener {

		public:
			ApplicationWrapper();
			~ApplicationWrapper();

			int run();
			
		protected:
			virtual int acceptConnection() = 0;
			virtual Engine::Serialize::SerializeManager *mgr() = 0;
#ifdef MADGINE_CLIENT_BUILD
			virtual Engine::Input::InputHandler *input() = 0;
#endif

			// Inherited via AppControl
			virtual void shutdownImpl() override;
		
			// Inherited via AppControl
			virtual void onApplicationConnected() override;

			virtual bool frameStarted(float timeSinceLastFrame) override;
			virtual bool frameRenderingQueued(float timeSinceLastFrame) override;
			virtual bool frameEnded(float timeSinceLastFrame) override;

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

			Engine::Serialize::Serialized<Util> mUtil;
			Engine::Serialize::Serialized<LauncherLogListener> mLog;
			Engine::Serialize::Serialized<ModuleLoader> mLoader;
			Engine::Serialize::Serialized<Inspector> mInspector;

			Shared::ApplicationInfo mAppInfo;
			bool mHaveAppInfo;

#ifdef MADGINE_CLIENT_BUILD
			Engine::App::OgreAppSettings mSettings;
			std::unique_ptr<Engine::App::OgreApplication> mApplication;
#endif
#ifdef MADGINE_SERVER_BUILD
			Engine::App::ServerAppSettings mSettings;
			std::unique_ptr<Engine::Server::ServerBase> mServer;
#endif			

			bool mRunning;			
			bool mStartRequested;
			
			
		};

	}
}
