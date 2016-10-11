#include "libinclude.h"
#include "ceguilauncher.h"
#include "UI/windownames.h"

#include "ceguiexception.h"

#include "App/configset.h"

#include "Ogre/myscenemanager.h"

#include "Database/exceptionmessages.h"

#include "ceguihelper.h"

#include "Windows.h"

#include "CeguiWindow.h"
#include "CeguiTextureDrawer.h"
#include "CeguiCheckbox.h"
#include "CeguiCombobox.h"
#include "CeguiTabWindow.h"
#include "CeguiTextbox.h"
#include "CeguiButton.h"
#include "CeguiLayout.h"
#include "CeguiLabel.h"
#include "CeguiBar.h"


namespace Engine {
	namespace GUI {
		namespace Cegui {
			CEGUILauncher::CEGUILauncher() :
				mKeepingCursorPos(false)
			{

				mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

				mSystem = &CEGUI::System::getSingleton();
				mContext = &mSystem->getDefaultGUIContext();

				mSchemeMgr = &CEGUI::SchemeManager::getSingleton();
				mImageMgr = &CEGUI::ImageManager::getSingleton();
				mWinMgr = &CEGUI::WindowManager::getSingleton();

				CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
				CEGUI::Font::setDefaultResourceGroup("Fonts");
				CEGUI::Scheme::setDefaultResourceGroup("Schemes");
				CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
				CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

				mSchemeMgr->createFromFile("TaharezLook.scheme");
				mSchemeMgr->createFromFile("FirstGui.scheme");
				mSchemeMgr->createFromFile("WindowsLook.scheme");

				mRootWindow = mWinMgr->createWindow("DefaultWindow", WindowNames::rootWindow);


				mGameImage = (CEGUI::BasicImage *)(&mImageMgr->create("BasicImage",
					WindowNames::gameImage));
				mGameImage->setAutoScaled(CEGUI::ASM_Both);


				mContext->setRootWindow(mRootWindow);

				mContext->getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
				mContext->getMouseCursor().hide();



			}

			CEGUILauncher::~CEGUILauncher()
			{
				CEGUI::OgreRenderer::destroySystem();
			}



			CEGUI::Texture *CEGUILauncher::createTexture(const std::string &name, Ogre::TexturePtr &ogreTex, bool take_ownership)
			{
				mRenderer->destroyTexture(name);
				return &mRenderer->createTexture(name, ogreTex, take_ownership);
			}




			CEGUI::Window *CEGUILauncher::getWindowByName(const std::string &windowName)
			{
				if (windowName == WindowNames::rootWindow)
					return mRootWindow;
				CEGUI::Window *w = mRootWindow->getChildRecursive(windowName);
				if (!w)
					throw CeguiException(Database::Exceptions::windowNotFound(windowName));
				return w;
			}

			CEGUI::Window *CEGUILauncher::getActiveWindow()
			{
				return mRootWindow->getActiveChild();
			}



			void CEGUILauncher::renderSingleFrame()
			{
				mSystem->renderAllGUIContexts();
			}

			Window *CEGUILauncher::loadLayout(const std::string &layout, const std::string &parent, Class _class)
			{
				CEGUI::Window *w = mWinMgr->loadLayoutFromFile(layout + ".layout");
				getWindowByName(parent)->addChild(w);
				w->hide();
				return map(w, _class);
			}




			CEGUI::MouseButton CEGUILauncher::convertButton(Engine::GUI::MouseButton buttonID)
			{
				switch (buttonID) {
				case Engine::GUI::LEFT_BUTTON:
					return CEGUI::LeftButton;

				case Engine::GUI::RIGHT_BUTTON:
					return CEGUI::RightButton;

				case Engine::GUI::MIDDLE_BUTTON:
					return CEGUI::MiddleButton;

				default:
					return CEGUI::LeftButton;
				}
			}

			Engine::GUI::MouseButton CEGUILauncher::convertButton(CEGUI::MouseButton buttonID) {
				switch (buttonID) {
				case CEGUI::LeftButton:
					return Engine::GUI::LEFT_BUTTON;
				case CEGUI::RightButton:
					return Engine::GUI::RIGHT_BUTTON;
				case CEGUI::MiddleButton:
					return Engine::GUI::MIDDLE_BUTTON;
				default:
					return Engine::GUI::LEFT_BUTTON;
				}
			}

			std::function<bool(const CEGUI::EventArgs&)> CEGUILauncher::convert(std::function<void(GUI::MouseEventArgs&)> f)
			{
				return [=](const CEGUI::EventArgs& args) {
					const CEGUI::MouseEventArgs &me = static_cast<const CEGUI::MouseEventArgs&>(args);
					const CEGUI::Sizef &parentSize = me.window->getPixelSize();
					const CEGUI::Vector2f &localPosition = CEGUI::CoordConverter::screenToWindow(*me.window, me.position);
					GUI::MouseEventArgs evt({ localPosition.d_x / parentSize.d_width, localPosition.d_y / parentSize.d_height }, { me.moveDelta.d_x / parentSize.d_height, me.moveDelta.d_y / parentSize.d_height }, me.wheelChange, convertButton(me.button));
					f(evt);
					return true;
				};
			}

			std::function<bool(const CEGUI::EventArgs&)> CEGUILauncher::convert(std::function<bool(GUI::KeyEventArgs&)> f)
			{
				return [=](const CEGUI::EventArgs& args) {
					const CEGUI::KeyEventArgs &me = static_cast<const CEGUI::KeyEventArgs&>(args);
					GUI::KeyEventArgs evt((GUI::Key) me.scancode);
					return f(evt);
				};
			}

			std::function<bool(const CEGUI::EventArgs&)> CEGUILauncher::convert(std::function<void()> f) {
				return [=](const CEGUI::EventArgs& args) {
					f();
					return true;
				};
			}


			void CEGUILauncher::injectKeyPress(const GUI::KeyEventArgs & arg)
			{
				mContext->injectKeyDown((CEGUI::Key::Scan)arg.scancode);
				//mContext->injectChar((CEGUI::Key::Scan)arg.text);
			}

			void CEGUILauncher::injectKeyRelease(const GUI::KeyEventArgs & arg)
			{
				mContext->injectKeyUp((CEGUI::Key::Scan)arg.scancode);
			}

			void CEGUILauncher::injectMousePress(const GUI::MouseEventArgs & arg)
			{
				mContext->injectMouseButtonDown(convertButton(arg.button));
			}

			void CEGUILauncher::injectMouseRelease(const GUI::MouseEventArgs & arg)
			{
				mContext->injectMouseButtonUp(convertButton(arg.button));
			}

			void CEGUILauncher::injectMouseMove(const GUI::MouseEventArgs & arg)
			{
				// Scroll wheel.
				if (arg.scrollWheel)
					mContext->injectMouseWheelChange(arg.scrollWheel);

				mContext->injectMouseMove(arg.moveDelta.x, arg.moveDelta.y);
			}

			void CEGUILauncher::injectTimePulse(float time)
			{
			}

			void CEGUILauncher::notifyDisplaySizeChanged(const Ogre::Vector2 & size)
			{
				mRenderer->setDisplaySize({ size.x, size.y });
				GUISystem::notifyDisplaySizeChanged(size);
			}

			bool CEGUILauncher::isCursorVisible()
			{
				return mContext->getMouseCursor().isVisible();
			}

			GUI::Window * CEGUILauncher::getWindow(const std::string & name, GUI::Class _class, GUI::Window * window)
			{
				//if (name == WindowNames::rootWindow) return map(mRootWindow, _class);
				CEGUI::Window* w = window ? map((CeguiWindow*)window) : mRootWindow;
				if (name == w->getName()) return map(w, _class);
				return map(w->getChildRecursive(name), _class);

			}

			GUI::Window *CEGUILauncher::createWindow(const std::string & name, Class _class)
			{
				std::string type;
				switch (_class) {
				case WINDOW_CLASS:
					type = "DefaultWindow";
					break;
				case TEXTUREDRAWER_CLASS:
					type = "TaharezLook/StaticImage";
					break;
				default:
					throw 0;
				}
				return map(mWinMgr->createWindow(type, name), _class);
			}

			void CEGUILauncher::setCursorPosition(const Ogre::Vector2 & pos)
			{
				mContext->getMouseCursor().setPosition({ pos.x, pos.y });
			}

			Ogre::Vector2 CEGUILauncher::getCursorPosition()
			{
				CEGUI::Vector2f pos = mContext->getMouseCursor().getPosition();
				return{ pos.d_x, pos.d_y };
			}

			void CEGUILauncher::setCursorVisibility(bool b)
			{
				mContext->getMouseCursor().setVisible(b);
			}


			CEGUI::Window * CEGUILauncher::map(CeguiWindow *w)
			{
				return w->window();
			}

			Window * CEGUILauncher::map(CEGUI::Window *w, Class _class)
			{
				auto it = mWindows.find(w);
				Window* wnd;
				if (it == mWindows.end()) {
					wnd = new CeguiWindow(w, this);
				}
				else {
					wnd = it->second;
				}

				if (!wnd->hasClass(_class)) {
					switch (_class) {
					case WINDOW_CLASS:
						break;
					case TEXTUREDRAWER_CLASS:
						wnd = new CeguiTextureDrawer(wnd, this);
						break;
					case CHECKBOX_CLASS:
						wnd = new CeguiCheckbox(wnd);
						break;
					case COMBOBOX_CLASS:
						wnd = new CeguiCombobox(wnd);
						break;
					case TABWINDOW_CLASS:
						wnd = new CeguiTabWindow(wnd);
						break;
					case TEXTBOX_CLASS:
						wnd = new CeguiTextbox(wnd);
						break;
					case BUTTON_CLASS:
						wnd = new CeguiButton(wnd);
						break;
					case LAYOUT_CLASS:
						wnd = new CeguiLayout(wnd);
						break;
					case LABEL_CLASS:
						wnd = new CeguiLabel(wnd);
						break;
					case BAR_CLASS:
						wnd = new CeguiBar(wnd);
						break;
					default:
						throw 0;
						break;
					}
				}
				mWindows[w] = wnd;
				return wnd;


			}

			Window * CEGUILauncher::map(Window::WindowId w)
			{
				return map((CEGUI::Window*)w, WINDOW_CLASS);
			}

			bool CEGUILauncher::isMapped(Window::WindowId w)
			{
				return mWindows.find((CEGUI::Window*)w) != mWindows.end();
			}

			std::list<Window::WindowId> CEGUILauncher::internalChildren(Window::WindowId w)
			{
				std::list<Window::WindowId> result;
				CEGUI::Window *win = (CEGUI::Window*)w;
				for (int i = 0; i < win->getChildCount(); ++i) {
					result.push_back((Window::WindowId)win->getChildAtIdx(i));
				}
				return result;
			}

			Window::WindowId CEGUILauncher::internalRoot()
			{
				return (Window::WindowId)mRootWindow;
			}

			/*void CEGUILauncher::destroyWindow(WindowId window)
			{
			}*/


		}
	}
}
