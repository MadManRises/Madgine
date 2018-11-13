#include "../ogrelib.h"

#include "myguitoplevelwindow.h"

#include "myguisystem.h"

#include "myguibar.h"
#include "myguibutton.h"
#include "myguicheckbox.h"
#include "myguicombobox.h"
#include "myguiimage.h"
#include "myguilabel.h"
#include "myguiscenewindow.h"
#include "myguitabwidget.h"
#include "myguitextbox.h"
#include "myguiemptywidget.h"

#include "Madgine/gui/widgetnames.h"

#include "Interfaces/window/windowapi.h"

namespace Engine
{
	namespace GUI
	{
		MyGUITopLevelWindow::MyGUITopLevelWindow(MyGUISystem& system) :
			TopLevelWindow(system),
			mWindow(nullptr),
			mScrollWheel(0),
			mCamera(nullptr),
			mViewport(nullptr),
			mInputManager(nullptr),
			mPointerManager(nullptr),
			mRenderManager(nullptr)
		{
			Ogre::Root *root = &Ogre::Root::getSingleton();

			

			std::map<std::string, std::string> parameters;

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
			parameters["externalWindowHandle"] = std::to_string((size_t)window()->mHandle);
			parameters["parentWindowHandle"] = std::to_string((size_t)window()->mHandle);
#endif

#if defined(Q_OS_MAC)
			parameters["macAPI"] = "cocoa";
			parameters["macAPICocoaUseNSView"] = "true";
#endif

			mWindow = root->createRenderWindow("", window()->width(), window()->height(), false, &parameters); // Create Application-Window

			

			Ogre::SceneManager *sceneM = root->createSceneManager(Ogre::ST_GENERIC, "GUISystemManager");
			mCamera = sceneM->createCamera("ContentCamera");
			mViewport = mWindow->addViewport(mCamera);


			mPlatform = std::make_unique<MyGUI::OgrePlatform>();
			mPlatform->initialise(mWindow, sceneM, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

			mGUI = std::make_unique<MyGUI::Gui>();
			mGUI->initialise("runTheme.xml");

			mLayoutManager = &MyGUI::LayoutManager::getInstance();
			mResourceManager = &MyGUI::ResourceManager::getInstance();
			mInputManager = &MyGUI::InputManager::getInstance();
			mRenderManager = &MyGUI::OgreRenderManager::getInstance();
			mPointerManager = &MyGUI::PointerManager::getInstance();


			//mResourceManager->load("Project.xml");

			mRootWindow = mGUI->createWidgetRealT("Widget", "PanelEmpty", { 0, 0, 1, 1 }, MyGUI::Align::Default, "Main",
				WidgetNames::rootWidget);

			//setCursorVisibility(false);

			Widget *loading = createTopLevelImage("Loading");
			Widget *progress = loading->createChildBar("ProgressBar");
			progress->setSize({ 0.8f,0,0,0,0.1f,0,0,0,1 });
			progress->setPos({ 0.1f,0,0,0,0.85f,0,0,0,0 });
			Widget *loadMsg = loading->createChildLabel("LoadingMsg");
			Widget *ingame = createTopLevelWidget("ingame");
			Widget *game = ingame->createChildSceneWindow("game");
			game->setSize({ 0.8f,0,0,0,1,0,0,0,1 });
			Widget *placeBaseButton = game->createChildButton("PlaceBaseButton");
			placeBaseButton->setPos({ 1,0,-120,0,0,30,0,0,0 });
			placeBaseButton->setSize({ 0,0,100,0,0,30,0,0,1 });
			Widget *endTurnButton = game->createChildButton("EndTurnButton");
			endTurnButton->setPos({ 1,0,-120,0,0,30,0,0,0 });
			endTurnButton->setSize({ 0,0,100,0,0,30,0,0,1 });
			Widget *unitUI = ingame->createChildWidget("UnitUI");
			unitUI->setPos({ 0.8f,0,0,0,0,0,0,0,0 });
			unitUI->setSize({ 0.2f,0,0,0,1,0,0,0,1 });
			Widget *fieldImage = unitUI->createChildImage("FieldImage");
			fieldImage->setPos({ 0, 0, 0, 0, 0, 0, 0, 0, 0 });
			fieldImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
			Widget *defenderImage = unitUI->createChildImage("DefenderImage");
			defenderImage->setPos({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 0 });
			defenderImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
			Widget *defenderHealth = defenderImage->createChildBar("DefenderHealth");
			defenderHealth->setPos({ 0, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
			defenderHealth->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *defenderExp = defenderImage->createChildBar("DefenderExp");
			defenderExp->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			defenderExp->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *attackerImage = unitUI->createChildImage("AttackerImage");
			attackerImage->setPos({ 0, 0, 0, 0.5f, 0, 0, 0, 0, 0 });
			attackerImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
			Widget *attackerHealth = attackerImage->createChildBar("AttackerHealth");
			attackerHealth->setPos({ 0, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
			attackerHealth->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *attackerExp = attackerImage->createChildBar("AttackerExp");
			attackerExp->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			attackerExp->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *modifierImage = unitUI->createChildImage("ModifierImage");
			modifierImage->setPos({ 1.15f, 0, 0, 0.15f, 0, 0, 0, 0, 0 });
			modifierImage->setSize({ 0.2f, 0, 0, 0.2f, 0, 0, 0, 0, 1 });
			Image *attackerStrengthIcon = unitUI->createChildImage("AttackerStrengthIcon");
			attackerStrengthIcon->setPos({ 0.025f, 0, 0, 1.025f, 0, 0, 0, 0, 0 });
			attackerStrengthIcon->setSize({ 0.15f, 0, 0, 0.15f, 0, 0, 0, 0, 1 });
			attackerStrengthIcon->setImage("Sword-01.png");
			Widget *attackerStrength = unitUI->createChildLabel("AttackerStrength");
			attackerStrength->setPos({ 0.2f, 0, 0, 1.025f, 0, 0, 0, 0, 0 });
			attackerStrength->setSize({ 0.2f, 0, 0, 0.15f, 0, 0, 0, 0, 1 });
			Widget *strengthArrow = unitUI->createChildImage("StrengthArrow");
			strengthArrow->setPos({ 0.4f, 0, 0, 1.075f, 0, 0, 0, 0, 0 });
			strengthArrow->setSize({ 0.2f, 0, 0, 0.05f, 0, 0, 0, 0, 1 });
			Widget *defenderStrength = unitUI->createChildLabel("DefenderStrength");
			defenderStrength->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			defenderStrength->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *defenderStrengthIcon = unitUI->createChildImage("DefenderStrengthIcon");
			defenderStrengthIcon->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			defenderStrengthIcon->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *contentWindow = unitUI->createChildWidget("ContentWindow");
			contentWindow->setPos({ 0, 0, 0, 1, 0, 0, 0, 0, 0 });
			contentWindow->setSize({ 1, 0, 0, -1, 1, 0, 0, 0, 1 });
			Widget *movementIcon = unitUI->createChildImage("MovementIcon");
			movementIcon->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			movementIcon->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *movement = unitUI->createChildLabel("Movement");
			movement->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			movement->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *health = unitUI->createChildLabel("Health");
			health->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			health->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *mainMenu = createTopLevelWidget("MainMenu");
			Widget *optionsButton = mainMenu->createChildButton("OptionsButton");
			optionsButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
			optionsButton->setPos({ 0.55f,0,0,0,0.55f,0,0,0,0 });
			Widget *findMatchButton = mainMenu->createChildButton("FindMatchButton");
			findMatchButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
			findMatchButton->setPos({ 0.55f,0,0,0,0.8f,0,0,0,0 });
			Widget *playButton = mainMenu->createChildButton("PlayButton");
			playButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
			playButton->setPos({ 0.05f,0,0,0,0.55f,0,0,0,0 });
			Widget *quitButton = mainMenu->createChildButton("QuitButton");
			quitButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
			quitButton->setPos({ 0.05f,0,0,0,0.8f,0,0,0,0 });
			Image *titleScreen = mainMenu->createChildImage("noname");
			titleScreen->setImage("img/MainMenu/TitleScreen.png");
			titleScreen->setSize({ 0.8f,0,0,0.1f,0,0,0,0,1 });
			titleScreen->setPos({ 0.1f,0,0,0,0.05f,0,0,0,0 });
			Widget *connectionFailureWindow = createTopLevelWidget("ConnectionFailureWindow");
			Widget *connectionLabel = connectionFailureWindow->createChildLabel("ConnectionLabel");
			Widget *retryButton = connectionFailureWindow->createChildButton("RetryButton");
			Widget *abortButton = connectionFailureWindow->createChildButton("AbortButton");
			Widget *lobbyMenu = createTopLevelWidget("LobbyMenu");
			Widget *startGameButton = lobbyMenu->createChildButton("StartGameButton");
			Widget *lobbyListMenu = createTopLevelWidget("LobbyListMenu");
			Widget *createLobbyButton = lobbyListMenu->createChildButton("CreateLobbyButton");
			Widget *joinLobbyButton = lobbyListMenu->createChildButton("JoinLobbyButton");
			Widget *backButton = lobbyListMenu->createChildButton("BackButton");
			Widget *lobbyList = lobbyListMenu->createChildCombobox("LobbyList");

		}

		MyGUITopLevelWindow::~MyGUITopLevelWindow()
		{
			mGUI->shutdown();
			mPlatform->shutdown();
		}



		void MyGUITopLevelWindow::injectKeyPress(const Input::KeyEventArgs& arg)
		{
			mInputManager->injectKeyPress(MyGUI::KeyCode::Enum(arg.scancode), arg.text);
		}

		void MyGUITopLevelWindow::injectKeyRelease(const Input::KeyEventArgs& arg)
		{
			mInputManager->injectKeyRelease(MyGUI::KeyCode::Enum(arg.scancode));
		}

		void MyGUITopLevelWindow::injectMousePress(const Input::MouseEventArgs& arg)
		{
			mInputManager->injectMousePress(arg.position[0], arg.position[1], MyGUISystem::convertButton(arg.button));
		}

		void MyGUITopLevelWindow::injectMouseRelease(const Input::MouseEventArgs& arg)
		{
			mInputManager->injectMouseRelease(arg.position[0], arg.position[1], MyGUISystem::convertButton(arg.button));
		}

		void MyGUITopLevelWindow::injectMouseMove(const Input::MouseEventArgs& arg)
		{
			mScrollWheel += arg.scrollWheel;
			mMoveDelta = arg.moveDelta;
			mMousePosition = arg.position;
			mInputManager->injectMouseMove(arg.position[0], arg.position[1], mScrollWheel);
		}

		Vector2 MyGUITopLevelWindow::relativeMoveDelta(MyGUI::Widget* w) const
		{
			return { mMoveDelta.x / w->getWidth(), mMoveDelta.y / w->getHeight() };
		}

		Vector2 MyGUITopLevelWindow::widgetRelative(MyGUI::Widget* w, int left, int top) const
		{
			if (left == -1)
				left = mMousePosition.x;
			if (top == -1)
				top = mMousePosition.y;
			float x = static_cast<float>(left) - w->getPosition().left;
			float y = static_cast<float>(top) - w->getPosition().top;
			return { x / w->getWidth(), y / w->getHeight() };
		}

		bool MyGUITopLevelWindow::isCursorVisible()
		{
			return mPointerManager->isVisible();
		}

		void MyGUITopLevelWindow::setCursorVisibility(bool v)
		{
			mPointerManager->setVisible(v);
		}

		void MyGUITopLevelWindow::setCursorPosition(const Vector2& pos)
		{
			mInputManager->injectMouseMove(pos.x, pos.y, 0);
		}

		Vector2 MyGUITopLevelWindow::getCursorPosition()
		{
			const MyGUI::IntPoint& p = mInputManager->getMousePosition();
			return { static_cast<float>(p.left), static_cast<float>(p.top) };
		}


		Vector3 MyGUITopLevelWindow::getScreenSize()
		{
			const MyGUI::IntSize& size = mRenderManager->getViewSize();
			return { static_cast<float>(size.width), static_cast<float>(size.height), 1 };
		}

		bool MyGUITopLevelWindow::singleFrame()
		{
			mWindow->update();
			return true;
		}

		void MyGUITopLevelWindow::onResize(size_t width, size_t height)
		{
			mWindow->windowMovedOrResized();
			((Ogre::WindowEventListener*)mRenderManager)->windowResized(mWindow);
			mCamera->setAspectRatio(Ogre::Real(width) / Ogre::Real(height));
			TopLevelWindow::onResize(width, height);
		}

		std::unique_ptr<Widget> MyGUITopLevelWindow::createWidget(const std::string& name)
		{
			return std::make_unique<MyGUIEmptyWidget>(name, *this);
		}

		std::unique_ptr<Bar> MyGUITopLevelWindow::createBar(const std::string &name)
		{
			return std::make_unique<MyGUIBar>(name, *this);
		}

		std::unique_ptr<Button> MyGUITopLevelWindow::createButton(const std::string &name)
		{
			return std::make_unique<MyGUIButton>(name, *this);
		}

		std::unique_ptr<Checkbox> MyGUITopLevelWindow::createCheckbox(const std::string &name)
		{
			return std::make_unique<MyGUICheckbox>(name, *this);
		}

		std::unique_ptr<Combobox> MyGUITopLevelWindow::createCombobox(const std::string &name)
		{
			return std::make_unique<MyGUICombobox>(name, *this);
		}

		std::unique_ptr<Label> MyGUITopLevelWindow::createLabel(const std::string &name)
		{
			return std::make_unique<MyGUILabel>(name, *this);
		}

		std::unique_ptr<SceneWindow> MyGUITopLevelWindow::createSceneWindow(const std::string &name)
		{
			return std::make_unique<MyGUISceneWindow>(name, *this);
		}

		std::unique_ptr<TabWidget> MyGUITopLevelWindow::createTabWidget(const std::string &name)
		{
			return std::make_unique<MyGUITabWidget>(name, *this);
		}

		std::unique_ptr<Textbox> MyGUITopLevelWindow::createTextbox(const std::string& name)
		{
			return std::make_unique<MyGUITextbox>(name, *this);
		}

		std::unique_ptr<Image> MyGUITopLevelWindow::createImage(const std::string &name)
		{
			return std::make_unique<MyGUIImage>(name, *this);
		}

		MyGUI::Widget* MyGUITopLevelWindow::rootWidget()
		{
			return mRootWindow;
		}

	}
}