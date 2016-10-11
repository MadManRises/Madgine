#include "libinclude.h"
#include "CeguiWindow.h"
#include "ceguilauncher.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiWindow::CeguiWindow(CEGUI::Window *window, CEGUILauncher *cegui) :
				mWindow(window),
				mCegui(cegui)
			{
			}


			CeguiWindow::~CeguiWindow()
			{
				for (CEGUI::Event::Connection &conn : mConnections) {
					conn->disconnect();
				}
			}

			void CeguiWindow::showModal()
			{
				mWindow->show();
				mWindow->moveToFront();
				mWindow->setModalState(true);
			}

			CEGUI::Window * CeguiWindow::window()
			{
				return mWindow;
			}

			bool CeguiWindow::isOpen()
			{
				return false;
			}

			void CeguiWindow::releaseInput()
			{
				mWindow->releaseInput();
			}

			void CeguiWindow::captureInput()
			{
				//mWindow->activate();
				mWindow->captureInput();
			}

			void CeguiWindow::hideModal()
			{
				mWindow->setModalState(false);
				mWindow->hide();
			}

			void CeguiWindow::show()
			{
				mWindow->show();
			}

			void CeguiWindow::hide()
			{
				mWindow->hide();
			}

			void CeguiWindow::activate()
			{
				mWindow->activate();
			}

			void CeguiWindow::destroy()
			{
				mWindow->destroy();
			}

			void CeguiWindow::setEnabled(bool b)
			{
				mWindow->setEnabled(b);
			}

			void CeguiWindow::addChild(Window * w)
			{
			}

			Window * CeguiWindow::getChild(const std::string & name, Class _class)
			{
				return mCegui->getWindow(name, _class, this);
			}


			void CeguiWindow::registerHandlerEvents(std::function<void(GUI::MouseEventArgs&)> mouseMove, std::function<void(GUI::MouseEventArgs&)> mouseDown, std::function<void(GUI::MouseEventArgs&)> mouseUp, std::function<void(GUI::MouseEventArgs&)> mouseScroll, std::function<bool(GUI::KeyEventArgs&)> keyPress)
			{

				mConnections.emplace_back(mWindow->subscribeEvent(CEGUI::Window::EventMouseMove,
					CEGUILauncher::convert(mouseMove)));
				mConnections.emplace_back(mWindow->subscribeEvent(CEGUI::Window::EventMouseButtonDown,
					CEGUILauncher::convert(mouseDown)));
				mConnections.emplace_back(mWindow->subscribeEvent(CEGUI::Window::EventMouseButtonUp,
					CEGUILauncher::convert(mouseUp)));
				mConnections.emplace_back(mWindow->subscribeEvent(CEGUI::Window::EventMouseWheel,
					CEGUILauncher::convert(mouseScroll)));
				mConnections.emplace_back(mWindow->subscribeEvent(CEGUI::Window::EventKeyUp,
					CEGUILauncher::convert(keyPress)));
			}


			void CeguiWindow::registerEvent(EventType type, std::function<void()> event)
			{
				CEGUI::String str;
				switch (type) {
				case ButtonClick:
					str = CEGUI::PushButton::EventClicked;
					break;
				case TextChanged:
					str = CEGUI::Editbox::EventTextChanged;
					break;
				case CloseClicked:
					str = CEGUI::FrameWindow::EventCloseClicked;
					break;
				case CheckboxToggled:
					str = CEGUI::ToggleButton::EventSelectStateChanged;
					break;
				case ComboboxSelectionChanged:
					str = CEGUI::Combobox::EventListSelectionAccepted;
					break;
				default:
					throw 0;
				}

				mConnections.emplace_back(mWindow->subscribeEvent(str, CEGUILauncher::convert(event)));
			}

			CeguiWindow::WindowSizeRelVector CeguiWindow::size()
			{
				return{ {mWindow->getPixelSize().d_width, 0, 0}, {mWindow->getPixelSize().d_height,0,0} };
			}

			Ogre::Vector2 CeguiWindow::pixelSize()
			{
				return{ mWindow->getPixelSize().d_width, mWindow->getPixelSize().d_height };
			}

			std::string CeguiWindow::name()
			{
				return mWindow->getName().c_str();
			}

			CeguiWindow::WindowId CeguiWindow::intern()
			{
				return (CeguiWindow::WindowId)mWindow;
			}

			bool CeguiWindow::isRoot()
			{
				return mWindow->getParent() == 0;
			}

			Ogre::Vector2 CeguiWindow::parentSize()
			{
				const CEGUI::Sizef &size = mWindow->getParent()->getPixelSize();
				return{ size.d_width, size.d_height };
			}

			void CeguiWindow::setPixelSize(const Ogre::Vector2 & size)
			{
				mWindow->setSize(CEGUI::USize(CEGUI::UDim(0, size.x), CEGUI::UDim(0, size.y)));
			}

			void CeguiWindow::setName(const std::string & text)
			{
				mWindow->setName(text);
			}

		}
	}
}


