#include "../ogrelib.h"

#include "myguisystem.h"

#include "Madgine/app/clientapplication.h"

#include "Madgine/app/clientappsettings.h"

#include "Madgine/gui/windownames.h"

#include "../input/oisinputhandler.h"

#include "Madgine/core/root.h"

#include "myguiwindow.h"

#include "myguibar.h"
#include "myguibutton.h"
#include "myguicheckbox.h"
#include "myguicombobox.h"
#include "myguiimage.h"
#include "myguilabel.h"
#include "myguiscenewindow.h"
#include "myguitabwindow.h"
#include "myguitextbox.h"
#include "myguiemptywindow.h"

extern "C" DLL_EXPORT Engine::GUI::GUISystem * guisystem(Engine::App::ClientApplication &app) { return new Engine::GUI::MyGUISystem(app); }

namespace Engine
{
	namespace GUI
	{
		MyGUISystem::MyGUISystem(App::ClientApplication& app) :
			GUISystem(app),
			mWindow(nullptr),
			mHwnd(nullptr),
			mInput(nullptr),
			mCamera(nullptr),
			mViewport(nullptr),
			mInputManager(nullptr),
			mPointerManager(nullptr),
			mRenderManager(nullptr),
			mScrollWheel(0)
		{
			const App::ClientAppSettings &settings = app.settings();

			Plugins::Plugin p("OgreMain" CONFIG_SUFFIX SHARED_LIBRARY_SUFFIX);

			p.load();

			mRoot = std::make_unique<Ogre::Root>((p.fullPath().parent_path() / "plugins" CONFIG_SUFFIX ".cfg").generic_string()); // Creating Root

			if (!mRoot->restoreConfig())
			{
				// if no existing config, or could not restore it, show the config dialog 
				if (!mRoot->showConfigDialog(nullptr))
				{
					throw 0;
				}
			}

			if (settings.mUseExternalSettings)
			{
				mRoot->initialise(false);

				mWindow = mRoot->createRenderWindow(settings.mWindowName, settings.mWindowWidth, settings.mWindowHeight,
					false, &settings.mWindowParameters);
			}
			else
			{
				mWindow = mRoot->initialise(true, settings.mWindowName); // Create Application-Window
			}

			if (settings.mInput) {
				mInput = settings.mInput;
			}
			else {
				mInputHolder = std::make_unique<Input::OISInputHandler>(mWindow);
				mInput = mInputHolder.get();
			}
			mInput->setSystem(this);

			mRoot->addFrameListener(this);

			mWindow->getCustomAttribute("WINDOW", &mHwnd);

			Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
		}

		MyGUISystem::~MyGUISystem()
		{
			Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
		}

		void MyGUISystem::injectKeyPress(const KeyEventArgs& arg)
		{
			mInputManager->injectKeyPress(MyGUI::KeyCode::Enum(arg.scancode), arg.text);
		}

		void MyGUISystem::injectKeyRelease(const KeyEventArgs& arg)
		{
			mInputManager->injectKeyRelease(MyGUI::KeyCode::Enum(arg.scancode));
		}

		void MyGUISystem::injectMousePress(const MouseEventArgs& arg)
		{
			mInputManager->injectMousePress(arg.position[0], arg.position[1], convertButton(arg.button));
		}

		void MyGUISystem::injectMouseRelease(const MouseEventArgs& arg)
		{
			mInputManager->injectMouseRelease(arg.position[0], arg.position[1], convertButton(arg.button));
		}

		void MyGUISystem::injectMouseMove(const MouseEventArgs& arg)
		{
			mScrollWheel += arg.scrollWheel;
			mMoveDelta = arg.moveDelta;
			mMousePosition = arg.position;
			mInputManager->injectMouseMove(arg.position[0], arg.position[1], mScrollWheel);
		}

		MyGUI::MouseButton MyGUISystem::convertButton(MouseButton::MouseButton buttonID)
		{
			switch (buttonID)
			{
			case MouseButton::LEFT_BUTTON:
				return MyGUI::MouseButton::Left;

			case MouseButton::RIGHT_BUTTON:
				return MyGUI::MouseButton::Right;

			case MouseButton::MIDDLE_BUTTON:
				return MyGUI::MouseButton::Middle;

			default:
				return MyGUI::MouseButton::Left;
			}
		}

		MouseButton::MouseButton MyGUISystem::convertButton(MyGUI::MouseButton buttonID)
		{
			switch (buttonID.getValue())
			{
			case MyGUI::MouseButton::Left:
				return MouseButton::LEFT_BUTTON;
			case MyGUI::MouseButton::Right:
				return MouseButton::RIGHT_BUTTON;
			case MyGUI::MouseButton::Middle:
				return MouseButton::MIDDLE_BUTTON;
			default:
				return MouseButton::LEFT_BUTTON;
			}
		}

		bool MyGUISystem::isCursorVisible()
		{
			return mPointerManager->isVisible();
		}

		void MyGUISystem::setCursorVisibility(bool v)
		{
			mPointerManager->setVisible(v);
		}

		void MyGUISystem::setCursorPosition(const Vector2& pos)
		{
			mInputManager->injectMouseMove(pos.x, pos.y, 0);
		}

		Vector2 MyGUISystem::getCursorPosition()
		{
			const MyGUI::IntPoint& p = mInputManager->getMousePosition();
			return { static_cast<float>(p.left), static_cast<float>(p.top) };
		}


		Vector2 MyGUISystem::getScreenSize()
		{
			const MyGUI::IntSize& size = mRenderManager->getViewSize();
			return { static_cast<float>(size.width), static_cast<float>(size.height) };
		}

		bool MyGUISystem::preInit()
		{

			for (const std::experimental::filesystem::path &p : app().root().resources().folders()) {
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
					p.generic_string(), "FileSystem", "General");
			}

			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

			Ogre::SceneManager *sceneM = mRoot->getSceneManager("SceneTmp");
			mCamera = sceneM->createCamera("ContentCamera");
			mViewport = mWindow->addViewport(mCamera);


			mPlatform = new MyGUI::OgrePlatform;
			mPlatform->initialise(mWindow, sceneM, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

			mGUI = new MyGUI::Gui;
			mGUI->initialise("runTheme.xml");

			mLayoutManager = &MyGUI::LayoutManager::getInstance();
			mResourceManager = &MyGUI::ResourceManager::getInstance();
			mInputManager = &MyGUI::InputManager::getInstance();
			mRenderManager = &MyGUI::OgreRenderManager::getInstance();
			mPointerManager = &MyGUI::PointerManager::getInstance();


			//mResourceManager->load("Project.xml");

			mRootWindow = mGUI->createWidgetRealT("Widget", "PanelEmpty", { 0, 0, 1, 1 }, MyGUI::Align::Default, "Main",
				WindowNames::rootWindow);

			setCursorVisibility(false);

			Window *loading = createTopLevelImage("Loading");
				Window *progress = loading->createChildBar("ProgressBar");
					progress->setSize({ 0.8f,0,0,0,0.1f,0,0,0,1 });
					progress->setPos({ 0.1f,0,0,0,0.85f,0,0,0,0 });
				Window *loadMsg = loading->createChildLabel("LoadingMsg");
			Window *ingame = createTopLevelWindow("ingame");
				Window *game = ingame->createChildSceneWindow("game");
					game->setSize({ 0.8f,0,0,0,1,0,0,0,1 });
					Window *placeBaseButton = game->createChildButton("PlaceBaseButton");
						placeBaseButton->setPos({ 1,0,-120,0,0,30,0,0,0 });
						placeBaseButton->setSize({ 0,0,100,0,0,30,0,0,1 });
					Window *endTurnButton = game->createChildButton("EndTurnButton");
						endTurnButton->setPos({ 1,0,-120,0,0,30,0,0,0 });
						endTurnButton->setSize({ 0,0,100,0,0,30,0,0,1 });
				Window *unitUI = ingame->createChildWindow("UnitUI");
					unitUI->setPos({ 0.8f,0,0,0,0,0,0,0,0 });
					unitUI->setSize({ 0.2f,0,0,0,1,0,0,0,1 });
					Window *fieldImage = unitUI->createChildImage("FieldImage");
					Window *defenderImage = unitUI->createChildImage("DefenderImage");
						Window *defenderHealth = defenderImage->createChildBar("DefenderHealth");
						Window *defenderExp = defenderImage->createChildBar("DefenderExp");
					Window *attackerImage = unitUI->createChildImage("AttackerImage");
						Window *attackerHealth = attackerImage->createChildBar("AttackerHealth");
						Window *attackerExp = attackerImage->createChildBar("AttackerExp");
					Window *modifierImage = unitUI->createChildImage("ModifierImage");
					Window *attackerStrengthIcon = unitUI->createChildImage("AttackerStrengthIcon");
					Window *attackerStrength = unitUI->createChildLabel("AttackerStrength");
					Window *strengthArrow = unitUI->createChildImage("StrengthArrow");
					Window *defenderStrength = unitUI->createChildLabel("DefenderStrength");
					Window *defenderStrengthIcon = unitUI->createChildImage("DefenderStrengthIcon");
					Window *contentWindow = unitUI->createChildWindow("ContentWindow");
					Window *movementIcon = unitUI->createChildImage("MovementIcon");
					Window *movement = unitUI->createChildLabel("Movement");
					Window *health = unitUI->createChildLabel("Health");
			Window *mainMenu = createTopLevelWindow("MainMenu");
				Window *optionsButton = mainMenu->createChildButton("OptionsButton");
					optionsButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
					optionsButton->setPos({ 0.55f,0,0,0,0.55f,0,0,0,0 });
				Window *findMatchButton = mainMenu->createChildButton("FindMatchButton");
					findMatchButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
					findMatchButton->setPos({ 0.55f,0,0,0,0.8f,0,0,0,0 });
				Window *playButton = mainMenu->createChildButton("PlayButton");
					playButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
					playButton->setPos({ 0.05f,0,0,0,0.55f,0,0,0,0 });
				Window *quitButton = mainMenu->createChildButton("QuitButton");
					quitButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
					quitButton->setPos({ 0.05f,0,0,0,0.8f,0,0,0,0 });
				Image *titleScreen = mainMenu->createChildImage("noname");
					titleScreen->setImage("img/MainMenu/TitleScreen.png");
					titleScreen->setSize({ 0.8f,0,0,0.1f,0,0,0,0,1 });
					titleScreen->setPos({ 0.1f,0,0,0,0.05f,0,0,0,0 });
			Window *connectionFailureWindow = createTopLevelWindow("ConnectionFailureWindow");
				Window *connectionLabel = connectionFailureWindow->createChildLabel("ConnectionLabel");
				Window *retryButton = connectionFailureWindow->createChildButton("RetryButton");
				Window *abortButton = connectionFailureWindow->createChildButton("AbortButton");
			Window *lobbyMenu = createTopLevelWindow("LobbyMenu");
				Window *startGameButton = lobbyMenu->createChildButton("StartGameButton");
			Window *lobbyListMenu = createTopLevelWindow("LobbyListMenu");
				Window *createLobbyButton = lobbyListMenu->createChildButton("CreateLobbyButton");
				Window *joinLobbyButton = lobbyListMenu->createChildButton("JoinLobbyButton");
				Window *backButton = lobbyListMenu->createChildButton("BackButton");
				Window *lobbyList = lobbyListMenu->createChildCombobox("LobbyList");
			
			calculateWindowGeometries();

			return GUISystem::preInit();
		}

		int MyGUISystem::go()
		{
			mRoot->startRendering();
			return 0;			
		}

		bool MyGUISystem::singleFrame()
		{
			mWindow->update();
			return true;
		}

		std::unique_ptr<Window> MyGUISystem::createWindow(const std::string& name)
		{
			return std::make_unique<MyGUIEmptyWindow>(name, *this);
		}

		std::unique_ptr<Bar> MyGUISystem::createBar(const std::string &name)
		{
			return std::make_unique<MyGUIBar>(name, *this);
		}

		std::unique_ptr<Button> MyGUISystem::createButton(const std::string &name)
		{
			return std::make_unique<MyGUIButton>(name, *this);
		}

		std::unique_ptr<Checkbox> MyGUISystem::createCheckbox(const std::string &name)
		{
			return std::make_unique<MyGUICheckbox>(name, *this);
		}

		std::unique_ptr<Combobox> MyGUISystem::createCombobox(const std::string &name)
		{
			return std::make_unique<MyGUICombobox>(name, *this);
		}

		std::unique_ptr<Label> MyGUISystem::createLabel(const std::string &name)
		{
			return std::make_unique<MyGUILabel>(name, *this);
		}

		std::unique_ptr<SceneWindow> MyGUISystem::createSceneWindow(const std::string &name)
		{
			return std::make_unique<MyGUISceneWindow>(name, *this);
		}

		std::unique_ptr<TabWindow> MyGUISystem::createTabWindow(const std::string &name)
		{
			return std::make_unique<MyGUITabWindow>(name, *this);
		}

		std::unique_ptr<Textbox> MyGUISystem::createTextbox(const std::string& name)
		{
			return std::make_unique<MyGUITextbox>(name, *this);
		}

		std::unique_ptr<Image> MyGUISystem::createImage(const std::string &name)
		{
			return std::make_unique<MyGUIImage>(name, *this);
		}

		MyGUI::Widget* MyGUISystem::rootWidget()
		{
			return mRootWindow;
		}

		bool MyGUISystem::frameStarted(const Ogre::FrameEvent& fe)
		{
			if (!sendFrameStarted(fe.timeSinceLastFrame))
				return false;

			if (mWindow->isClosed())
				return false;

			return true;
		}

		bool MyGUISystem::frameRenderingQueued(const Ogre::FrameEvent& fe)
		{
			Ogre::WindowEventUtilities::messagePump();
			if (mWindow->isClosed() || !sendFrameRenderingQueued(fe.timeSinceLastFrame))
			{
				return false;
			}

			try
			{
				//PROFILE("Input");
				mInput->update();
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("Unhandled Exception during Input!");
				LOG_EXCEPTION(e);
			}

			return true;
		}

		bool MyGUISystem::frameEnded(const Ogre::FrameEvent& fe)
		{
			if (!sendFrameEnded(fe.timeSinceLastFrame))
				return false;

			if (mWindow->isClosed())
				return false;

			return true;
		}

	}
}