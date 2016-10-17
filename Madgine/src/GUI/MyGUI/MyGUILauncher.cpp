#include "libinclude.h"

#include "MyGUILauncher.h"

#include "MyGUIWindow.h"

#include "GUI\Windows\WindowContainer.h"

#include "UI\windownames.h"

#include "Database\exceptionmessages.h"

#include "MyGUIWindow.h"

namespace Engine {
	namespace GUI {
		namespace MyGui {

			MyGUILauncher::MyGUILauncher(Ogre::RenderWindow *window, Ogre::SceneManager *sceneMgr) :
				mScrollWheel(0),
				GUISystem()
			{
				mCamera = sceneMgr->createCamera("ContentCamera");
				mViewport = window->addViewport(mCamera);
				

				mPlatform = std::make_unique<MyGUI::OgrePlatform>();
				mPlatform->initialise(window, sceneMgr);



			}

			MyGUILauncher::~MyGUILauncher()
			{
				printHierarchy();
				mRootWindow->finalize();
				delete mRootWindow;
				if (mGUI) {
					mGUI->shutdown();
					mGUI.reset();
				}
				mPlatform->shutdown();
				mPlatform.reset();
			}

			void MyGUILauncher::init()
			{
				mGUI = std::make_unique<MyGUI::Gui>();
				mGUI->initialise("runTheme.xml");

				mLayoutManager = &MyGUI::LayoutManager::getInstance();
				mResourceManager = &MyGUI::ResourceManager::getInstance();
				mInputManager = &MyGUI::InputManager::getInstance();
				mRenderManager = &MyGUI::OgreRenderManager::getInstance();
				

				mResourceManager->load("Project.xml");

				mInternRootWindow = mGUI->createWidgetRealT("Widget", "PanelEmpty", { 0, 0, 1, 1 }, MyGUI::Align::Default, "Main", WindowNames::rootWindow);

				mRootWindow = new MyGUIWindow(mInternRootWindow, this, 0);
				updateWindowSizes();

				setCursorVisibility(false);
			}

			void MyGUILauncher::injectKeyPress(const KeyEventArgs & arg)
			{
				mInputManager->injectKeyPress(MyGUI::KeyCode::Enum(arg.scancode), arg.text);
			}
			void MyGUILauncher::injectKeyRelease(const KeyEventArgs & arg)
			{
				mInputManager->injectKeyRelease(MyGUI::KeyCode::Enum(arg.scancode));
			}
			void MyGUILauncher::injectMousePress(const MouseEventArgs & arg)
			{
				mInputManager->injectMousePress(arg.position.x, arg.position.y, convertButton(arg.button));
			}
			void MyGUILauncher::injectMouseRelease(const MouseEventArgs & arg)
			{
				mInputManager->injectMouseRelease(arg.position.x, arg.position.y, convertButton(arg.button));
			}
			void MyGUILauncher::injectMouseMove(const MouseEventArgs & arg)
			{
				mScrollWheel += arg.scrollWheel;
				mMoveDelta = arg.moveDelta;
				mMousePosition = arg.position;
				mInputManager->injectMouseMove(arg.position.x, arg.position.y, mScrollWheel);
			}
			
			bool MyGUILauncher::isCursorVisible()
			{
				return MyGUI::PointerManager::getInstance().isVisible();
			}
			void MyGUILauncher::setCursorVisibility(bool v)
			{
				MyGUI::PointerManager::getInstance().setVisible(v);
			}
			void MyGUILauncher::setCursorPosition(const Ogre::Vector2 & pos)
			{
				MyGUI::InputManager::getInstance().injectMouseMove(pos.x, pos.y, 0);
			}
			Ogre::Vector2 MyGUILauncher::getCursorPosition()
			{
				const MyGUI::IntPoint &p = MyGUI::InputManager::getInstance().getMousePosition();
				return{ (float)p.left, (float)p.top };
			}
			

			Ogre::Vector2 MyGUILauncher::getScreenSize()
			{
				const MyGUI::IntSize &size = MyGUI::RenderManager::getInstance().getViewSize();
				return{ (float)size.width, (float)size.height };
			}


			Ogre::Vector2 MyGUILauncher::relativeMoveDelta(MyGUI::Widget * w)
			{
				return{ mMoveDelta.x / w->getWidth(), mMoveDelta.y / w->getHeight() };
			}

			void MyGUILauncher::destroy(MyGUI::Widget * w)
			{
				mGUI->destroyWidget(w);
			}


			MyGUI::MouseButton MyGUILauncher::convertButton(MouseButton::MouseButton buttonID)
			{
				switch (buttonID) {
				case Engine::GUI::MouseButton::LEFT_BUTTON:
					return MyGUI::MouseButton::Left;

				case Engine::GUI::MouseButton::RIGHT_BUTTON:
					return MyGUI::MouseButton::Right;

				case Engine::GUI::MouseButton::MIDDLE_BUTTON:
					return MyGUI::MouseButton::Middle;

				default:
					return MyGUI::MouseButton::Left;
				}
			}

			MouseButton::MouseButton MyGUILauncher::convertButton(MyGUI::MouseButton buttonID) {
				switch (buttonID.getValue()) {
				case MyGUI::MouseButton::Left:
					return Engine::GUI::MouseButton::LEFT_BUTTON;
				case MyGUI::MouseButton::Right:
					return Engine::GUI::MouseButton::RIGHT_BUTTON;
				case MyGUI::MouseButton::Middle:
					return Engine::GUI::MouseButton::MIDDLE_BUTTON;
				default:
					return Engine::GUI::MouseButton::LEFT_BUTTON;
				}
			}

			Ogre::Vector2 MyGUILauncher::widgetRelative(MyGUI::Widget * w, int left, int top)
			{
				if (left == -1) 
					left = mMousePosition.x;
				if (top == -1)
					top = mMousePosition.y;
				float x = (float)left - w->getPosition().left;
				float y = (float)top - w->getPosition().top;
				return{ x / w->getWidth(), y / w->getHeight() };
			}

		}
	}
}