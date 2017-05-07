#include "madginelib.h"

#include "MyGUILauncher.h"

#include "MyGUIWindow.h"

#include "GUI\Windows\WindowContainer.h"

#include "..\windownames.h"


#include "MyGUIWindow.h"

#ifdef _MSC_VER
#pragma warning (push, 0)
#endif
#include <MYGUI\MyGUI.h>
#include <MYGUI\MyGUI_OgrePlatform.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif


namespace Engine {
	namespace GUI {
		namespace MyGui {

			MyGUILauncher::MyGUILauncher(Ogre::RenderWindow *window, Ogre::SceneManager *sceneMgr) :
				mGUI(nullptr),
				mPlatform(nullptr),
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

				mInternRootWindow = mGUI->createWidgetRealT("Widget", "PanelEmpty", { 0, 0, 1, 1 }, MyGUI::Align::Default, "Main", WindowNames::rootWindow);

				mRootWindow = new MyGUIWindow(mInternRootWindow, this, 0);

				setCursorVisibility(false);

				return true;
			}

			void MyGUILauncher::finalize()
			{
				if (mRootWindow) {
					mRootWindow->finalize();
					delete mRootWindow;
				}
				if (mGUI) {
					mGUI->shutdown();
					delete mGUI;
				}
				GUISystem::finalize();
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
				mInputManager->injectMousePress(arg.position[0], arg.position[1], convertButton(arg.button));
			}
			void MyGUILauncher::injectMouseRelease(const MouseEventArgs & arg)
			{
				mInputManager->injectMouseRelease(arg.position[0], arg.position[1], convertButton(arg.button));
			}
			void MyGUILauncher::injectMouseMove(const MouseEventArgs & arg)
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


			std::array<float, 2> MyGUILauncher::relativeMoveDelta(MyGUI::Widget * w)
			{
				return{ {mMoveDelta.x / w->getWidth(), mMoveDelta.y / w->getHeight() } };
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

			std::array<float, 2> MyGUILauncher::widgetRelative(MyGUI::Widget * w, int left, int top)
			{
				if (left == -1) 
					left = mMousePosition.x;
				if (top == -1)
					top = mMousePosition.y;
				float x = (float)left - w->getPosition().left;
				float y = (float)top - w->getPosition().top;
				return{ { x / w->getWidth(), y / w->getHeight() } };
			}

		}
	}
}