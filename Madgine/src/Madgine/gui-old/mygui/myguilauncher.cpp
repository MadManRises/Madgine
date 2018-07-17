#include "../../clientlib.h"

#include "myguilauncher.h"

#include "myguiwindow.h"

#include "../windows/windowcontainer.h"

#include "../windownames.h"

#include "../../app/clientappsettings.h"

#include "../../input/oisinputhandler.h"

#include "../../app/application.h"

#include "../../ui/uimanager.h"

#include "../../app/clientapplication.h"

#ifdef _MSC_VER
#pragma warning (push, 0)
#endif
#include <MYGUI/MyGUI.h>
#include <MYGUI/MyGUI_OgrePlatform.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include "../../core/root.h"


namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			MyGUILauncher::MyGUILauncher(App::ClientApplication &app) :
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

			bool MyGUILauncher::preInit()
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


				//mResourceManager->load("Project.xml");

				mInternRootWindow = mGUI->createWidgetRealT("Widget", "PanelEmpty", {0, 0, 1, 1}, MyGUI::Align::Default, "Main",
				                                            WindowNames::rootWindow);

				mRootWindow = new MyGUIWindow(mInternRootWindow, this, nullptr);

				setCursorVisibility(false);

				return GUISystem::preInit();
			}

			void MyGUILauncher::finalize()
			{
				GUISystem::finalize();

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

			}

			int MyGUILauncher::go()
			{
				mRoot->startRendering();
				return 0;
			}

			bool MyGUILauncher::singleFrame()
			{
				mWindow->update();
				return true;
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
				if (!sendFrameStarted(fe.timeSinceLastFrame))
					return false;

				if (mWindow->isClosed())
					return false;

				return true;
			}

			bool MyGUILauncher::frameRenderingQueued(const Ogre::FrameEvent& fe)
			{
				Ogre::WindowEventUtilities::messagePump();
				if (mWindow->isClosed() || !sendFrameRenderingQueued(fe.timeSinceLastFrame, ui().currentContext()))
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
				if (!sendFrameEnded(fe.timeSinceLastFrame))
					return false;

				if (mWindow->isClosed())
					return false;

				return true;
			}


		}
	}
}
