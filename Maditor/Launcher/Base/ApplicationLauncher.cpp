
#include "madginelib.h"

#include "ApplicationLauncher.h"

#include "ModuleLoader.h"

#include "Watcher\LogWatcher.h"

#include "Shared.h"

#include "Network\networkmanager.h"

		ApplicationLauncher::ApplicationLauncher() :
			ProcessTalker("Maditor_Launcher", "Launcher"),
			mRunning(true),
			mApplication(0),
			mAppInfo(SharedMemory::getSingleton().mAppInfo),
			mWatcher()
		{
			load();
		}
		ApplicationLauncher::~ApplicationLauncher()
		{
			sendMsg(APP_SHUTDOWN);
			delete mApplication;
			mApplication = 0;
			mWatcher.clear();
			sendMsg(APP_AFTER_SHUTDOWN);
			delete mOgreLog;
		}

		void ApplicationLauncher::load()
		{

			mSettings.mInput = &mInput;
			mSettings.mUseExternalSettings = true;
			mSettings.mWindowName = "QtOgre";
			mSettings.mWindowWidth = mAppInfo.mWindowWidth;
			mSettings.mWindowHeight = mAppInfo.mWindowHeight;
			mSettings.mRootDir = mAppInfo.mMediaDir.c_str();
			mSettings.mPluginsFile = mSettings.mRootDir + "plugins.cfg";
			mSettings.mUseNetwork = true;

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


			mApplication = new Engine::App::Application;
				
			mApplication->setup(mSettings);

			mInput.setSystem(&Engine::GUI::GUISystem::getSingleton());
			std::string project = mAppInfo.mProjectDir.c_str();
			sendMsg(APP_CREATED);

			mOgreLog = new LogWatcher("Madgine.log", project);
			
			mLoader.setup(project + "debug/bin/", project + "debug/runtime/");
			mApplication->init();
			sendMsg(APP_INITIALIZED);

			Engine::Network::NetworkManager::getSingleton().startServer(1000);

			Ogre::Root::getSingleton().addFrameListener(this);

		}


		void ApplicationLauncher::start()
		{
			sendMsg(APP_STARTED);
			mApplication->go();
			sendMsg(APP_STOPPED);
		}

		void ApplicationLauncher::stop()
		{			
			mApplication->shutdown();
		}

		

		void ApplicationLauncher::receiveMessage(const ApplicationMsg & msg)
		{
			switch (msg.mCmd) {
			case START_APP:
				start();
				break;
			case STOP_APP:
				stop();
				break;
			case SHUTDOWN:
				stop();
				mRunning = false;
				break;
			case RESIZE_WINDOW:
				resizeWindow();
				break;
			}
		}

		void ApplicationLauncher::resizeWindow()
		{
			Engine::App::Application::getSingleton().resizeWindow();
		}


		bool ApplicationLauncher::frameRenderingQueued(const Ogre::FrameEvent & fe)
		{
			PROFILE("Launcher", "Frame");
			mWatcher.update();
			update();
			mLoader.update();
			return true;
		}

		/*Watcher::InputWrapper * ApplicationLauncher::input()
		{
			return &mInput;
		}*/

		int ApplicationLauncher::exec()
		{
			
			while (mRunning) {
				mWatcher.update();
				update();
				//mLoader->update();		
			}
			return 0;
		}

		bool ApplicationLauncher::sendMsg(ApplicationCmd cmd)
		{
			ApplicationMsg msg;
			msg.mCmd = cmd;
			return ProcessTalker::sendMsg(msg, "Maditor");
		}

	