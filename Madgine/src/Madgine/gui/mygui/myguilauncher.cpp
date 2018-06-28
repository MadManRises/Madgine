#include "../../clientlib.h"

#include "myguilauncher.h"

#include "myguiwindow.h"

#include "../windows/windowcontainer.h"

#include "../windownames.h"

#include "../../app/ogreappsettings.h"

#include "../../scene/ogrescenemanager.h"

#include "../../input/oisinputhandler.h"

#include "../../app/application.h"

#include "../../app/configset.h"

#ifdef _MSC_VER
#pragma warning (push, 0)
#endif
#include <MYGUI/MyGUI.h>
#include <MYGUI/MyGUI_OgrePlatform.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include "../../app/root.h"


namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			MyGUILauncher::MyGUILauncher(App::Application &app, const App::OgreAppSettings &settings) :
				GUISystem(app),
				mGUI(nullptr),
				mPlatform(nullptr), mLayoutManager(nullptr), mResourceManager(nullptr), mInputManager(nullptr),
				mRenderManager(nullptr),
				mInternRootWindow(nullptr),
				mScrollWheel(0),
				mWindow(nullptr),
				mHwnd(nullptr),
				mInput(nullptr)
			{

				Plugins::Plugin p("OgreMain" CONFIG_SUFFIX SHARED_LIBRARY_SUFFIX);

				p.load();

				std::cout << p.fullPath().parent_path().generic_string() << std::endl;

				mRoot = std::make_unique<Ogre::Root>((p.fullPath().parent_path() / "plugins" CONFIG_SUFFIX ".cfg").generic_string()); // Creating Root

				mConfig = std::make_unique<App::ConfigSet>(*this, mRoot.get(), "config.vs"); // Loading Config and configuring Root

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

				mWindow->update();

				mRoot->addFrameListener(this);

				mWindow->getCustomAttribute("WINDOW", &mHwnd);

				//mGUI = OGRE_MAKE_UNIQUE_FUNC(GUI::Cegui::CEGUILauncher, GUI::GUISystem)();
				Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);


				

				if (settings.mInput) {
					mInput = settings.mInput;
				}
				else {
					mInputHolder = std::make_unique<Input::OISInputHandler>(mWindow);
					mInput = mInputHolder.get();
				}
				mInput->setSystem(this);

			}

			MyGUILauncher::~MyGUILauncher()
			{
			

				Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);

			}

			bool MyGUILauncher::init()
			{
				if (!GUISystem::init())
					return false;


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


				//mResourceManager->load("Project.xml");

				mInternRootWindow = mGUI->createWidgetRealT("Widget", "PanelEmpty", {0, 0, 1, 1}, MyGUI::Align::Default, "Main",
				                                            WindowNames::rootWindow);

				mRootWindow = new MyGUIWindow(mInternRootWindow, this, nullptr);

				setCursorVisibility(false);

				mConfig->applyLanguage(); // Set the Language in the Config to all Windows

				return true;
			}

			void MyGUILauncher::finalize()
			{
				if (mRootWindow)
				{
					mRootWindow->finalize();
					delete mRootWindow;
				}
				if (mGUI)
				{
					mGUI->shutdown();
					delete mGUI;
				}
				if (mPlatform) {
					mPlatform->shutdown();
					delete mPlatform;
				}

				mCamera->getSceneManager()->destroyCamera(mCamera);

				return GUISystem::finalize();
			}

			int MyGUILauncher::go()
			{
				mRoot->startRendering();
				return 0;
			}

			void MyGUILauncher::renderSingleFrame()
			{
				mWindow->update();
			}

			void MyGUILauncher::injectKeyPress(const KeyEventArgs& arg)
			{
				mInputManager->injectKeyPress(MyGUI::KeyCode::Enum(arg.scancode), arg.text);
			}

			void MyGUILauncher::injectKeyRelease(const KeyEventArgs& arg)
			{
				mInputManager->injectKeyRelease(MyGUI::KeyCode::Enum(arg.scancode));
			}

			void MyGUILauncher::injectMousePress(const MouseEventArgs& arg)
			{
				mInputManager->injectMousePress(arg.position[0], arg.position[1], convertButton(arg.button));
			}

			void MyGUILauncher::injectMouseRelease(const MouseEventArgs& arg)
			{
				mInputManager->injectMouseRelease(arg.position[0], arg.position[1], convertButton(arg.button));
			}

			void MyGUILauncher::injectMouseMove(const MouseEventArgs& arg)
			{
				mScrollWheel += arg.scrollWheel;
				mMoveDelta = Ogre::Vector2(arg.moveDelta.data());
				mMousePosition = Ogre::Vector2(arg.position.data());
				mInputManager->injectMouseMove(arg.position[0], arg.position[1], mScrollWheel);
			}

			bool MyGUILauncher::isCursorVisible()
			{
				return MyGUI::PointerManager::getInstance().isVisible();
			}

			void MyGUILauncher::setCursorVisibility(bool v)
			{
				MyGUI::PointerManager::getInstance().setVisible(v);
			}

			void MyGUILauncher::setCursorPosition(const Ogre::Vector2& pos)
			{
				MyGUI::InputManager::getInstance().injectMouseMove(pos.x, pos.y, 0);
			}

			Ogre::Vector2 MyGUILauncher::getCursorPosition()
			{
				const MyGUI::IntPoint& p = MyGUI::InputManager::getInstance().getMousePosition();
				return {static_cast<float>(p.left), static_cast<float>(p.top)};
			}


			Ogre::Vector2 MyGUILauncher::getScreenSize()
			{
				const MyGUI::IntSize& size = MyGUI::RenderManager::getInstance().getViewSize();
				return {static_cast<float>(size.width), static_cast<float>(size.height)};
			}


			std::array<float, 2> MyGUILauncher::relativeMoveDelta(MyGUI::Widget* w) const
			{
				return {{mMoveDelta.x / w->getWidth(), mMoveDelta.y / w->getHeight()}};
			}

			void MyGUILauncher::destroy(MyGUI::Widget* w)
			{
				mGUI->destroyWidget(w);
			}


			MyGUI::MouseButton MyGUILauncher::convertButton(MouseButton::MouseButton buttonID)
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

			MouseButton::MouseButton MyGUILauncher::convertButton(MyGUI::MouseButton buttonID)
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

			std::array<float, 2> MyGUILauncher::widgetRelative(MyGUI::Widget* w, int left, int top) const
			{
				if (left == -1)
					left = mMousePosition.x;
				if (top == -1)
					top = mMousePosition.y;
				float x = static_cast<float>(left) - w->getPosition().left;
				float y = static_cast<float>(top) - w->getPosition().top;
				return {{x / w->getWidth(), y / w->getHeight()}};
			}

			void MyGUILauncher::setWindowProperties(bool fullscreen, unsigned int width, unsigned int height)
			{
				mWindow->setFullscreen(fullscreen, width, height);
				resizeWindow();
			}

			void MyGUILauncher::resizeWindow()
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

			bool MyGUILauncher::frameStarted(const Ogre::FrameEvent& fe)
			{
				if (!app().sendFrameStarted(fe.timeSinceLastFrame))
					return false;

				if (mWindow->isClosed())
					return false;

				return true;
			}

			bool MyGUILauncher::frameRenderingQueued(const Ogre::FrameEvent& fe)
			{
				if (mWindow->isClosed() || !app().sendFrameRenderingQueued(fe.timeSinceLastFrame))
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

			bool MyGUILauncher::frameEnded(const Ogre::FrameEvent& fe)
			{
				if (!app().sendFrameEnded(fe.timeSinceLastFrame))
					return false;

				if (mWindow->isClosed())
					return false;

				return true;
			}


		}
	}
}
