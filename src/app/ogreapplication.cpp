#include "../clientlib.h"

#include "ogreapplication.h"
#include "ogreappsettings.h"
#include "../gui/mygui/myguilauncher.h"

#include "../scene/ogrescenemanager.h"

#include "../ui/uimanager.h"
#include "configset.h"
#include "../resources/resourceloader.h"
#include "../util/profile.h"



#include "OgreWindowEventUtilities.h"


namespace Engine
{
	namespace App
	{
		OgreApplication::OgreApplication(Plugins::PluginManager &pluginMgr) :
			Application(this, pluginMgr),
			mPaused(false)
		{
		}

		OgreApplication::~OgreApplication()
		{
		}

		void OgreApplication::setup(const OgreAppSettings& settings)
		{
			mSettings = &settings;

			// Initialise GUISystem 
			mGUI = std::make_unique<GUI::MyGui::MyGUILauncher>(*this, settings);

			mLoader = std::make_unique<Resources::ResourceLoader>(this, mSettings->mRootDir);

			Application::setup(settings);

			// Create SceneManagerBase
			mSceneMgr = Serialize::make_noparent_unique<Scene::OgreSceneManager>(*this, &Ogre::Root::getSingleton());

			// Create UIManager
			mUI = std::make_unique<UI::UIManager>(*mGUI.get());

			
		}

		bool OgreApplication::init()
		{
			mLoader->loadScripts();

			if (!Application::init())
				return false;
			
			if (!mGUI->init())
				return false;

			if (!mUI->preInit())
				return false;
			
			std::optional<Scripting::ArgumentList> res = callMethodIfAvailable("afterViewInit");
			if (res && !res->empty() && (!res->front().is<bool>() || !res->front().as<bool>()))
				return false;

			if (!mLoader->load())
				return false;


			if (!mSceneMgr->init())
				return false; // Initialise all Scene-Components


			if (!mUI->init())
				return false; // Initialise all Handler


			return true;
		}

		void OgreApplication::finalize()
		{
			mUI->finalize();

			mGUI->finalize();

			mSceneMgr->finalize();

			Application::finalize();
		}

		int OgreApplication::go()
		{
			Application::go();
			mPaused = false;

			if (!callMethodCatch("main"))
			{
				return -1;
			}

			int result = mGUI->go();

			clear();

			return result;
		}

		void OgreApplication::shutdown()
		{
			//mConfig->save();
			Application::shutdown();
		}


		bool OgreApplication::update(float timeSinceLastFrame)
		{
			Ogre::WindowEventUtilities::messagePump();
			if (!mPaused)
			{
				try
				{
					PROFILE("UIManager");
					mUI->update(timeSinceLastFrame);
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during UI-Update!");
					LOG_EXCEPTION(e);
				}

				try
				{
					PROFILE("SceneManager");
					mSceneMgr->update(timeSinceLastFrame, mUI->currentContext());
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during Scene-Update!");
					LOG_EXCEPTION(e);
				}

				try
				{
					PROFILE("GUI");
					mGUI->update(timeSinceLastFrame);
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during GUI-Update!");
					LOG_EXCEPTION(e);
				}
			}
			return Application::update(timeSinceLastFrame);
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

		bool OgreApplication::singleFrame(float timeSinceLastFrame)
		{
			mGUI->renderSingleFrame();
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


		void OgreApplication::clear()
		{
			mSceneMgr->clear();
			Application::clear();
			mUI->clear();
		}

		void OgreApplication::renderFrame()
		{
			mGUI->renderSingleFrame();
		}

		KeyValueMapList OgreApplication::maps()
		{
			return Application::maps().merge(mSceneMgr, mUI);
		}
	}
}
