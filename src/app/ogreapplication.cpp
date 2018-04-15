#include "../ogrelib.h"

#include "ogreapplication.h"
#include "ogreappsettings.h"
#include "../gui/mygui/myguilauncher.h"

#include "../scene/ogrescenemanager.h"

#include "../ui/uimanager.h"
#include "configset.h"
#include "../resources/resourceloader.h"
#include "../util/profile.h"

#include "../input/oisinputhandler.h"

#include "OgreWindowEventUtilities.h"


#include "../util/standardlog.h"

namespace Engine
{

	namespace App
	{
		OgreApplication::OgreApplication() :
			Application(this),
			mPaused(false),
			mWindow(nullptr),
			mHwnd(nullptr),
			mInput(nullptr)
		{
		}

		OgreApplication::~OgreApplication()
		{
			Util::UtilMethods::setup(nullptr);
		}

		void OgreApplication::setup(const OgreAppSettings& settings)
		{
			mLog = std::make_unique<Util::StandardLog>(settings.mAppName);
			Util::UtilMethods::setup(mLog.get());

			mSettings = &settings;

			_setupOgre();

			if (mSettings->mUseExternalSettings)
			{
				mRoot->initialise(false);

				mWindow = mRoot->createRenderWindow(mSettings->mWindowName, mSettings->mWindowWidth, mSettings->mWindowHeight,
				                                    false, &mSettings->mWindowParameters);
			}
			else
			{
				mWindow = mRoot->initialise(true, mSettings->mWindowName); // Create Application-Window
			}

			mWindow->getCustomAttribute("WINDOW", &mHwnd);

			mLoader = std::make_unique<Resources::ResourceLoader>(this, mSettings->mRootDir);

			Application::setup(settings);

			// Create SceneManagerBase
			mSceneMgr = Serialize::make_noparent_unique<Scene::OgreSceneManager>(*this, mRoot.get());

			mWindow->update();

			// Initialise GUISystem 
			mGUI = std::make_unique<GUI::MyGui::MyGUILauncher>(*this, mWindow, mSceneMgr->getSceneManager());
			//mGUI = OGRE_MAKE_UNIQUE_FUNC(GUI::Cegui::CEGUILauncher, GUI::GUISystem)();
			Ogre::WindowEventUtilities::addWindowEventListener(mWindow, mGUI.get());

			// Create UIManager
			mUI = std::make_unique<UI::UIManager>(*mGUI.get());

			if (mSettings->mInput) {
				mSettings->mInput->setSystem(mGUI.get());
				mInput = mSettings->mInput;
			}
			else {
				mInputHolder = std::make_unique<Input::OISInputHandler>(mWindow);
				mInput = mInputHolder.get();
				mInput->setSystem(mGUI.get());
			}

			
		}

		bool OgreApplication::init()
		{
			mLoader->loadScripts();

			if (!Application::init())
				return false;

			if (!mGUI->init())
				return false;

			mRoot->addFrameListener(this);

			if (!mUI->preInit())
				return false;

			std::pair<bool, Scripting::ArgumentList> res = callMethodIfAvailable("afterViewInit");
			if (res.first && !res.second.empty() && (!res.second.front().is<bool>() || !res.second.front().as<bool>()))
				return false;

			if (!mLoader->load())
				return false;


			if (!mSceneMgr->init())
				return false; // Initialise all Scene-Components


			if (!mUI->init())
				return false; // Initialise all Handler


			mConfig->applyLanguage(); // Set the Language in the Config to all Windows

			return true;
		}

		void OgreApplication::finalize()
		{
			mUI->finalize();

			Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, mGUI.get());
			mGUI->finalize();

			mSceneMgr->finalize();

			Application::finalize();
		}

		int OgreApplication::go()
		{
			Application::go();
			mPaused = false;

			if (!callMethodCatch("main").first)
			{
				return -1;
			}

			mRoot->startRendering();

			clear();

			return 0;
		}

		void OgreApplication::shutdown()
		{
			mConfig->save();
			Application::shutdown();
		}

		bool OgreApplication::frameStarted(const Ogre::FrameEvent& fe)
		{
			if (!sendFrameStarted(fe.timeSinceLastFrame))
				return false;

			if (mWindow->isClosed())
				return false;

			return true;
		}


		bool OgreApplication::update(float timeSinceLastFrame)
		{
			Ogre::WindowEventUtilities::messagePump();
			return mRoot->renderOneFrame();
		}

		Scene::SceneManagerBase& OgreApplication::sceneMgr()
		{
			return *mSceneMgr;
		}

		Scene::SceneComponentBase& OgreApplication::getSceneComponent(size_t i)
		{
			return mSceneMgr->getComponent(i);
		}

		UI::GuiHandlerBase& OgreApplication::getGuiHandler(size_t i)
		{
			return mUI->getGuiHandler(i);
		}

		UI::GameHandlerBase& OgreApplication::getGameHandler(size_t i)
		{
			return mUI->getGameHandler(i);
		}

		bool OgreApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
		{
			if (mWindow->isClosed() || !Application::update(fe.timeSinceLastFrame))
			{
				return false;
			}

			if (!mPaused)
			{
				try
				{
					PROFILE("UIManager");
					mUI->update(fe.timeSinceLastFrame);
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during UI-Update!");
					LOG_EXCEPTION(e);
				}

				try
				{
					PROFILE("SceneManager");
					mSceneMgr->update(fe.timeSinceLastFrame, mUI->currentContext());
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during Scene-Update!");
					LOG_EXCEPTION(e);
				}

				try
				{
					PROFILE("GUI");
					mGUI->update(fe.timeSinceLastFrame);
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during GUI-Update!");
					LOG_EXCEPTION(e);
				}

				try
				{
					PROFILE("Input");
					mInput->update();
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during Input!");
					LOG_EXCEPTION(e);
				}
			}

			return true;
		}

		bool OgreApplication::frameEnded(const Ogre::FrameEvent& fe)
		{
			if (!sendFrameEnded(fe.timeSinceLastFrame))
				return false;

			if (mWindow->isClosed())
				return false;

			return true;
		}

		bool OgreApplication::fixedUpdate(float timeStep)
		{
			if (!mPaused)
			{
				try
				{
					PROFILE("SceneManager");
					mSceneMgr->fixedUpdate(timeStep, mUI->currentContext());
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during Scene-Update!");
					LOG_EXCEPTION(e);
				}

				try
				{
					PROFILE("UIManager");
					mUI->fixedUpdate(timeStep);
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during UI-Update!");
					LOG_EXCEPTION(e);
				}
			}

			return true;
		}


		void OgreApplication::setWindowProperties(bool fullscreen, unsigned int width, unsigned int height)
		{
			mWindow->setFullscreen(fullscreen, width, height);
			resizeWindow();
		}

		Ogre::RenderWindow* OgreApplication::renderWindow()
		{
			return mWindow;
		}


		void OgreApplication::clear()
		{
			mSceneMgr->clear();
			Application::clear();
			mUI->clear();
		}

		void OgreApplication::_setupOgre()
		{
			mRoot = std::make_unique<Ogre::Root>(mSettings->mPluginsFile); // Creating Root

			mConfig = std::make_unique<ConfigSet>(*this, mRoot.get(), "config.vs"); // Loading Config and configuring Root
		}

		void OgreApplication::resizeWindow()
		{
			if (mWindow)
			{
				mWindow->windowMovedOrResized();
				Ogre::WindowEventUtilities::WindowEventListeners::iterator
					start = Ogre::WindowEventUtilities::_msListeners.lower_bound(mWindow),
					end = Ogre::WindowEventUtilities::_msListeners.upper_bound(mWindow);
				for (Ogre::WindowEventUtilities::WindowEventListeners::iterator index = start; index != end; ++index)
					index->second->windowResized(mWindow);
			}
		}

		void OgreApplication::renderFrame()
		{
			mGUI->renderSingleFrame();
			mWindow->update();
		}

		KeyValueMapList OgreApplication::maps()
		{
			return Application::maps().merge(mSceneMgr, mUI);
		}
	}
}
