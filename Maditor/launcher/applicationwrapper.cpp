#include "maditorlauncherlib.h"

#include "applicationwrapper.h"

#include "shared/sharedmemory.h"

#include "Madgine/serialize/serializemanager.h"


#ifdef MADGINE_SERVER_BUILD
#include "Madgine/server/serverbase.h"
#else
#include "Madgine/input/inputhandler.h"
#include "Madgine/gui/guisystem.h"
#endif

#include "shared/errorcodes.h"


#include <iostream>
#include "Madgine/scripting/types/apihelper.h"

#include "Madgine/util/log.h"

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

			switch (mAppInfo.mType)
			{
#ifdef MADGINE_CLIENT_BUILD
			case Shared::CLIENT_LAUNCHER:
			{

				mSettings.mRootDir = mAppInfo.mDataDir + "Media/";
				mSettings.mPluginsFile = mAppInfo.mDataDir + "plugins.cfg";

				mSettings.mInput = input();
				mSettings.mUseExternalSettings = true;
				mSettings.mWindowName = "QtOgre";
				mSettings.mWindowWidth = mAppInfo.mWindowWidth;
				mSettings.mWindowHeight = mAppInfo.mWindowHeight;
				mSettings.mAppName = mAppInfo.mAppName;

				Ogre::NameValuePairList &parameters = mSettings.mWindowParameters;

				/*
				Flag within the parameters set so that Ogre3D initializes an OpenGL context on it's own.
				*/
				parameters["currentGLContext"] = Ogre::String("false");

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
				parameters["externalWindowHandle"] = Ogre::StringConverter::toString(mAppInfo.mWindowHandle);
				parameters["parentWindowHandle"] = Ogre::StringConverter::toString(mAppInfo.mWindowHandle);
#endif

#if defined(Q_OS_MAC)
				parameters["macAPI"] = "cocoa";
				parameters["macAPICocoaUseNSView"] = "true";
#endif

				mLoader->pluginMgr().getPlugin("OgreMadgine").load();

				mApplication = std::make_unique<Engine::App::OgreApplication>(mLoader->pluginMgr());

				mApplication->setGlobalMethod("print", &ApplicationWrapper::lua_log);

				mApplication->setup(mSettings);
				mUtil->setApp(mApplication.get());

				mLog->init();

				mApplication->log().addListener(mLog.ptr());




				Ogre::LogManager::getSingleton().getLog("Ogre.log")->addListener(mLog.ptr());
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
#endif

#ifdef MADGINE_SERVER_BUILD
			case Shared::SERVER_LAUNCHER:
				{
					mServer = std::unique_ptr<Engine::Server::ServerBase>(
						mLoader->createServer(mAppInfo.mServerClass, mAppInfo.mAppName, mAppInfo.mDataDir + "Media/"));
					if (!mServer)
						return Shared::FAILED_CREATE_SERVER_CLASS;

					mServer->setGlobalMethod("print", &ApplicationWrapper::lua_log);

					mLog->init();

					mServer->log().addListener(mLog.ptr());

					mServer->addFrameListener(this);
					applicationConnected({});
					result = mServer->run();
					mRunning = false;
				}
				break;

#endif
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
#ifdef MADGINE_SERVER_BUILD
			mInspector->init(*mServer.get());
#elif MADGINE_CLIENT_BUILD
			mInspector->init(*mApplication.get());
#endif
		}

		bool ApplicationWrapper::frameRenderingQueued(float timeSinceLastFrame)
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
#ifdef MADGINE_CLIENT_BUILD
				mApplication->shutdown();
#else
			mServer->shutdown();
#endif
		}

		void ApplicationWrapper::pauseImpl()
		{
		}

		void ApplicationWrapper::resizeWindowImpl()
		{
#ifdef MADGINE_CLIENT_BUILD
			if (mApplication)
				mApplication->gui().resizeWindow();
#endif
		}

		void ApplicationWrapper::execLuaImpl(const std::string& cmd)
		{
			mExecOutBuffer.str("");

			Engine::Scripting::GlobalScopeBase* scope;
			switch (mAppInfo.mType)
			{
#ifdef MADGINE_CLIENT_BUILD
			case Shared::CLIENT_LAUNCHER: scope = mApplication.get(); break;
#endif
#ifdef MADGINE_SERVER_BUILD
			case Shared::SERVER_LAUNCHER: scope = mServer.get();
				break;
#endif
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


