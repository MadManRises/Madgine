#include "maditorlauncherlib.h"

#include "applicationwrapper.h"

#include "../shared/sharedmemory.h"

#include "Madgine/serialize/serializemanager.h"



#include "Madgine/server/serverbase.h"


#include "Madgine/input/inputhandler.h"
#include "Madgine/gui/guisystem.h"


#include "../shared/errorcodes.h"


#include <iostream>
#include "Madgine/scripting/types/apihelper.h"

#include "Madgine/util/log.h"

#include "Madgine/core/root.h"

namespace Maditor
{
	namespace Launcher
	{
		ApplicationWrapper* ApplicationWrapper::sInstance = nullptr;

		ApplicationWrapper::ApplicationWrapper() :
			SerializableUnit(Shared::AppControl::masterLauncher),
			mRunning(false),
			mStartRequested(false),
			mHaveAppInfo(false),
			mCurrentExecScope(nullptr)

		{
			sInstance = this;
			Engine::Serialize::Debugging::StreamDebugging::setLoggingEnabled(true);
		}

		ApplicationWrapper::~ApplicationWrapper()
		{
		}


		int ApplicationWrapper::run()
		{
			int e = acceptConnection();
			if (e != Shared::NO_ERROR_SHARED)
				return e;

			mHaveAppInfo = false;

			using namespace std::literals::chrono_literals;

			std::chrono::time_point<std::chrono::system_clock> end;

			end = std::chrono::system_clock::now() + 20000ms; // this is the end point

			while (std::chrono::system_clock::now() < end && !mHaveAppInfo) // still less than the end?
			{
				mgr()->receiveMessages(1);
				//appInfo.readState(mgr()->getSlaveStream());
			}

			if (!mHaveAppInfo)
				return Shared::APPLICATION_INFO_TIMEOUT;

			if (mAppInfo.mDebugged)
			{
#ifdef _WIN32
				end = std::chrono::system_clock::now() + 5000ms; // this is the end point
				while (!IsDebuggerPresent() &&
					!GetAsyncKeyState(VK_F10) &&
					std::chrono::system_clock::now() < end)
				{
					std::this_thread::yield();
				}
#endif
				if (std::chrono::system_clock::now() >= end)
				{
					return Shared::DEBUGGER_CONNECTION_TIMEOUT;
				}
			}

			int result = 0;


			mRunning = true;

			mLoader->setup(mAppInfo.mProjectDir + "debug/bin/");
			while (mLoader->receiving() && mRunning && mgr()->clientCount() == 1)
			{
				mgr()->sendAndReceiveMessages();
			}
			if (mgr()->clientCount() != 1 || !mRunning)
			{
				return Shared::MODULE_LOAD_FAILED;
			}

			mRootSettings.mMediaDir = mAppInfo.mDataDir + "Media/";

			mRoot = std::make_unique<Engine::Core::Root>(mRootSettings);

			mRoot->luaState().setGlobalMethod("print", &ApplicationWrapper::lua_log);

			mLog->init();

			if (mAppInfo.mType == Shared::CLIENT_LAUNCHER)
				mRoot->pluginMgr().getPlugin("OgreMadgine").load();
			else
				mRoot->pluginMgr().getPlugin("MadgineServer").load();


			if (!mRoot->init()) {
				return Shared::APP_INIT_FAILED;
			}

			switch (mAppInfo.mType)
			{
			case Shared::CLIENT_LAUNCHER:
			{

				mSettings.mInput = input();
				mSettings.mUseExternalSettings = true;
				mSettings.mWindowName = "QtOgre";
				mSettings.mWindowWidth = mAppInfo.mWindowWidth;
				mSettings.mWindowHeight = mAppInfo.mWindowHeight;
				mSettings.mAppName = mAppInfo.mAppName;

				std::map<std::string, std::string> &parameters = mSettings.mWindowParameters;

				/*
				Flag within the parameters set so that Ogre3D initializes an OpenGL context on it's own.
				*/
				parameters["currentGLContext"] = "false";

				/*
				We need to supply the low level OS window handle to this QWindow so that Ogre3D knows where to draw
				the scene. Below is a cross-platform method on how to do this.
				If you set both options (externalWindowHandle and parentWindowHandle) this code will work with OpenGL
				and DirectX.
				*/
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
				parameters["externalWindowHandle"] = Ogre::StringConverter::toString();
				parameters["parentWindowHandle"] = Ogre::StringConverter::toString((size_t)(target->winId()));
#else
				parameters["externalWindowHandle"] = std::to_string(mAppInfo.mWindowHandle);
				parameters["parentWindowHandle"] = std::to_string(mAppInfo.mWindowHandle);
#endif

#if defined(Q_OS_MAC)
				parameters["macAPI"] = "cocoa";
				parameters["macAPICocoaUseNSView"] = "true";
#endif



				mApplication = std::make_unique<Engine::App::ClientApplication>(*mRoot);

				mApplication->setup(mSettings);
				mUtil->setApp(mApplication.get());

				mApplication->log().addListener(mLog.ptr());


				//Ogre::LogManager::getSingleton().getLog("Ogre.log")->addListener(mLog.ptr());
				mApplication->addFrameListener(this);

				if (!mApplication->init()) {
					mApplication->finalize();
					return Shared::APP_INIT_FAILED;
				}
				applicationConnected({});

				while (mRunning) {
					mgr()->sendAndReceiveMessages();
					Engine::SignalSlot::ConnectionManager::getSingleton().update();
					if (mgr()->clientCount() != 1) {
						//net->close();
						return Shared::MADITOR_DISCONNECTED;
					}
					if (mStartRequested) {
						mApplication->go();
						mStartRequested = false;
						stop({});
					}
				}
				mApplication->finalize();
			}
				break;
			case Shared::SERVER_LAUNCHER:
				{
					

					mServer = std::unique_ptr<Engine::Server::ServerBase>(
						mLoader->createServer(mAppInfo.mServerClass, mAppInfo.mAppName, *mRoot));
					if (!mServer)
						return Shared::FAILED_CREATE_SERVER_CLASS;
											

					mServer->log().addListener(mLog.ptr());

					mServer->addFrameListener(this);
					applicationConnected({});
					result = mServer->go();
					mRunning = false;
				}
				break;
			default:
				return Shared::UNSUPPORTED_LAUNCHER_TYPE;
			}
			return result;
		}

		void ApplicationWrapper::shutdownImpl()
		{
			mRunning = false;
			stopImpl();
		}

		void ApplicationWrapper::onApplicationConnected()
		{
			if (mAppInfo.mType == Shared::CLIENT_LAUNCHER)
				mInspector->init(*mApplication.get());
			else
				mInspector->init(*mServer.get());
		}

		bool ApplicationWrapper::frameRenderingQueued(float timeSinceLastFrame, Engine::Scene::ContextMask context)
		{
			mUtil->profiler()->stopProfiling(); // PreRender

			mUtil->profiler()->startProfiling("Rendering");

			mgr()->sendAndReceiveMessages();
			if (mgr()->clientCount() != 1)
			{
				shutdownImpl();
			}
			return mRunning;
		}


		bool ApplicationWrapper::frameStarted(float timeSinceLastFrame)
		{
			mUtil->update();
			mUtil->profiler()->startProfiling("Frame");
			mUtil->profiler()->startProfiling("PreRender");

			return true;
		}

		bool ApplicationWrapper::frameEnded(float timeSinceLastFrame)
		{
			mUtil->profiler()->stopProfiling(); // Rendering
			mUtil->profiler()->stopProfiling(); // Frame

			return true;
		}


		void ApplicationWrapper::startImpl()
		{
			mStartRequested = true;
		}

		void ApplicationWrapper::stopImpl()
		{
			if (mAppInfo.mType == Shared::CLIENT_LAUNCHER)
				mApplication->shutdown();
			else
				mServer->shutdown();
		}

		void ApplicationWrapper::pauseImpl()
		{
		}

		void ApplicationWrapper::resizeWindowImpl()
		{
			/*if (mApplication)
				mApplication->gui().resizeWindow();*/
		}

		void ApplicationWrapper::execLuaImpl(const std::string& cmd)
		{
			mExecOutBuffer.str("");

			Engine::Scripting::GlobalScopeBase* scope;
			switch (mAppInfo.mType)
			{
			case Shared::CLIENT_LAUNCHER: scope = mApplication.get(); break;
			case Shared::SERVER_LAUNCHER: scope = mServer.get(); break;
			default:
				throw 0;
			}

			mCurrentExecScope = scope->lua_state();
			bool failure = false;
			try
			{
				scope->executeString(cmd);
			}
			catch (const Engine::Scripting::ScriptingException& e)
			{
				mExecOutBuffer << e.what() << std::endl;
				failure = true;
			}
			luaResponse(cmd, mExecOutBuffer.str(), failure, {});
		}

		void ApplicationWrapper::configImpl(const Shared::ApplicationInfo& info)
		{
			mAppInfo = info;
			mHaveAppInfo = true;
		}

		int ApplicationWrapper::lua_log(lua_State* state)
		{
			Engine::Scripting::ArgumentList args(state, Engine::Scripting::APIHelper::stackSize(state));
			for (const Engine::ValueType& v : args)
			{
				if (sInstance->mCurrentExecScope == state)
				{
					sInstance->mExecOutBuffer << v.toString() << std::endl;
				}
				else
				{
					std::cout << v.toString() << std::endl;
				}
			}

			return 0;
		}
	}
}


