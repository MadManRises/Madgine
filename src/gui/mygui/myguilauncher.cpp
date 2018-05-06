#include "../../clientlib.h"

#include "myguilauncher.h"

#include "myguiwindow.h"

#include "../windows/windowcontainer.h"

#include "../windownames.h"

#ifdef _MSC_VER
#pragma warning (push, 0)
#endif
#include <MYGUI/MyGUI.h>
#include <MYGUI/MyGUI_OgrePlatform.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif


namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			MyGUILauncher::MyGUILauncher(App::Application &app, Ogre::RenderWindow* window, Ogre::SceneManager* sceneMgr) :
				GUISystem(app),
				mGUI(nullptr),
				mPlatform(nullptr), mLayoutManager(nullptr), mResourceManager(nullptr), mInputManager(nullptr),
				mRenderManager(nullptr),
				mInternRootWindow(nullptr),
				mScrollWheel(0)
			{
				mCamera = sceneMgr->createCamera("ContentCamera");
				mViewport = window->addViewport(mCamera);

				mPlatform = new MyGUI::OgrePlatform;
				mPlatform->initialise(window, sceneMgr, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
			}

			MyGUILauncher::~MyGUILauncher()
			{
				mPlatform->shutdown();
				delete mPlatform;

				mCamera->getSceneManager()->destroyCamera(mCamera);
			}

			bool MyGUILauncher::init()
			{
				if (!GUISystem::init())
					return false;
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
				return GUISystem::finalize();
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
		}
	}
}
