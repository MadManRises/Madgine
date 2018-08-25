#include "../ogrelib.h"

#include "myguisystem.h"

#include "Madgine/app/clientapplication.h"

#include "Madgine/app/clientappsettings.h"

#include "Madgine/resources/resourcemanager.h"

#include "Madgine/core/root.h"

#include "myguiwidget.h"

#include "myguitoplevelwindow.h"



extern "C" DLL_EXPORT Engine::GUI::GUISystem * guisystem(Engine::App::ClientApplication &app) { return new Engine::GUI::MyGUISystem(app); }

namespace Engine
{
	namespace GUI
	{
		MyGUISystem::MyGUISystem(App::ClientApplication& app) :
			GUISystem(app)
		{
			const App::ClientAppSettings &settings = app.settings();

			Plugins::Plugin p("OgreMain" CONFIG_SUFFIX SHARED_LIBRARY_SUFFIX);

			p.load();

			mRoot = std::make_unique<Ogre::Root>((p.fullPath().parent_path() / "plugins" CONFIG_SUFFIX ".cfg").generic_string()); // Creating Root

			Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);

			if (!mRoot->restoreConfig())
			{
				// if no existing config, or could not restore it, show the config dialog 
				if (!mRoot->showConfigDialog(nullptr))
				{
					throw 0;
				}
			}

			mRoot->initialise(false);

			



			mRoot->addFrameListener(this);
		}

		MyGUISystem::~MyGUISystem()
		{
			
		}


		MyGUI::MouseButton MyGUISystem::convertButton(Input::MouseButton::MouseButton buttonID)
		{
			switch (buttonID)
			{
			case Input::MouseButton::LEFT_BUTTON:
				return MyGUI::MouseButton::Left;

			case Input::MouseButton::RIGHT_BUTTON:
				return MyGUI::MouseButton::Right;

			case Input::MouseButton::MIDDLE_BUTTON:
				return MyGUI::MouseButton::Middle;

			default:
				return MyGUI::MouseButton::Left;
			}
		}

		Input::MouseButton::MouseButton MyGUISystem::convertButton(MyGUI::MouseButton buttonID)
		{
			switch (buttonID.getValue())
			{
			case MyGUI::MouseButton::Left:
				return Input::MouseButton::LEFT_BUTTON;
			case MyGUI::MouseButton::Right:
				return Input::MouseButton::RIGHT_BUTTON;
			case MyGUI::MouseButton::Middle:
				return Input::MouseButton::MIDDLE_BUTTON;
			default:
				return Input::MouseButton::LEFT_BUTTON;
			}
		}

		
		bool MyGUISystem::init()
		{

			for (const std::experimental::filesystem::path &p : app().root().resources().folders()) {
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
					p.generic_string(), "FileSystem", "General");
			}

			addTopLevelWindow(std::make_unique<MyGUITopLevelWindow>(*this));

			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

			
			return GUISystem::init();
		}

		int MyGUISystem::go()
		{
			startLoop();
			mRoot->startRendering();
			return 0;			
		}

		bool MyGUISystem::frameStarted(const Ogre::FrameEvent& fe)
		{
			if (isShutdown() || !sendFrameStarted(std::chrono::microseconds{ static_cast<long long>(fe.timeSinceLastFrame * 1000000) }))
				return false;

			/*if (mWindow->isClosed())
				return false;
				*/
			return true;
		}

		bool MyGUISystem::frameRenderingQueued(const Ogre::FrameEvent& fe)
		{
			Ogre::WindowEventUtilities::messagePump();
			if (/*mWindow->isClosed() || */!sendFrameRenderingQueued(std::chrono::microseconds{ static_cast<long long>(fe.timeSinceLastFrame * 1000000) }))
			{
				return false;
			}

			

			return true;
		}

		bool MyGUISystem::frameEnded(const Ogre::FrameEvent& fe)
		{
			if (!sendFrameEnded(std::chrono::microseconds{ static_cast<long long>(fe.timeSinceLastFrame * 1000000) }))
				return false;

			/*if (mWindow->isClosed())
				return false;*/

			return true;
		}

		
		void MyGUISystem::messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage)
		{
			if (lml != Ogre::LML_CRITICAL) return;
			Engine::Util::MessageType level;
			switch (lml) {
			case Ogre::LML_CRITICAL:
				if (Ogre::StringUtil::startsWith(message, "WARNING:")) {
					LOG_WARNING(message.substr(strlen("WARNING: ")));
				}
				else
					LOG_ERROR(message);
				break;
			default:
				throw 0;
			}			
		}
	

	}
}