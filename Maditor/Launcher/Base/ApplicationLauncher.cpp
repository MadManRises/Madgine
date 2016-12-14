
#include "madginelib.h"

#include "ApplicationLauncher.h"

#include "ModuleLoader.h"

#include "Watcher\LogWatcher.h"

#include "Shared.h"

#include "Network\networkmanager.h"

		ApplicationLauncher::ApplicationLauncher() :
			mRunning(true),
			mAppInfo(SharedMemory::getSingleton().mAppInfo)
		{
			load();
		}
		ApplicationLauncher::~ApplicationLauncher()
		{

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

				
			mApplication.setup(mSettings);

			mInput.setSystem(&Engine::GUI::GUISystem::getSingleton());
			std::string project = mAppInfo.mProjectDir.c_str();
			//sendMsg(APP_CREATED);
			
			mNetwork.addTopLevelItem(this);
			mNetwork.startServer(1000);
			while (mNetwork.clientCount() == 0) {
				mNetwork.receiveMessages();
				mNetwork.acceptConnections();
			}

			mLoader.setup(project + "debug/bin/", project + "debug/runtime/");
			mApplication.init();
			//sendMsg(APP_INITIALIZED);

			mNetwork.addTopLevelItem(&Engine::Scene::SceneManager::getSingleton());
			

			

			Ogre::Root::getSingleton().addFrameListener(this);

		}


		void ApplicationLauncher::start()
		{
			//sendMsg(APP_STARTED);
			mApplication.go();
			//sendMsg(APP_STOPPED);
		}

		void ApplicationLauncher::stop()
		{			
			mApplication.shutdown();
		}


		void ApplicationLauncher::resizeWindow()
		{
			Engine::App::Application::getSingleton().resizeWindow();
		}


		bool ApplicationLauncher::frameRenderingQueued(const Ogre::FrameEvent & fe)
		{
			PROFILE("Launcher", "Frame");
			mNetwork.receiveMessages();
			if (mNetwork.clientCount() < 1) {
				stop();
			}
			return true;
		}

		/*Watcher::InputWrapper * ApplicationLauncher::input()
		{
			return &mInput;
		}*/

		int ApplicationLauncher::exec()
		{
			
			while (mRunning) {								
				mNetwork.receiveMessages();
				if (mNetwork.clientCount() < 1) {
					mRunning = false;
				}
				//mLoader->update();		
			}
			return 0;
		}


	