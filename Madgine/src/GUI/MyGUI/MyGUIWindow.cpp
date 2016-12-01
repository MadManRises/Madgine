#include "madginelib.h"

#include "GUI\Windows\window.h"


#include "MyGUIWindow.h"
#include "MyGUILauncher.h"

#include "MyGUILabel.h"
#include "MyGUIBar.h"
#include "MyGUITextureDrawer.h"
#include "MyGUITextbox.h"
#include "MyGUIButton.h"
#include "MyGUICheckbox.h"
#include "MyGUICombobox.h"

#include "Util\UtilMethods.h"
#include "Database\exceptionmessages.h"

namespace Engine {
	namespace GUI {
		namespace MyGui {

			MyGUIWindow::MyGUIWindow(MyGUI::Widget *window, MyGUILauncher *gui, WindowContainer *parent) :
				WindowContainer(gui, parent, window->getName()),
				mWindow(window),
				mGui(gui)
			{

				if (window->isUserString("Size")) {
					std::stringstream ss;
					ss.str(window->getUserString("Size"));
					float xw, xh, xa, yw, yh, ya;
					ss >> xw >> xh >> xa >> yw >> yh >> ya;
					setSize({ { xw, xh, xa },{ yw, yh, ya } });
				}
				else {
					setSize({ { 1, 0, 0 } ,{ 0, 1, 0 } });
				}

				if (window->isUserString("Pos")) {
					std::stringstream ss;
					ss.str(window->getUserString("Pos"));
					float xw, xh, xa, yw, yh, ya;
					ss >> xw >> xh >> xa >> yw >> yh >> ya;
					setPos({ { xw, xh, xa },{ yw, yh, ya } });
				}
				else {
					setPos({ { 0, 0, 0 } ,{ 0, 0, 0 } });
				}

				mWindow->eventKeyButtonReleased += MyGUI::newDelegate(this, &MyGUIWindow::handleKeyEvent);

				mWindow->setNeedMouseFocus(false);
				mWindow->setNeedKeyFocus(false);
			}

			MyGUIWindow::~MyGUIWindow()
			{
				mGui->destroy(mWindow);
			}

			void MyGUIWindow::registerHandlerEvents(void * id, std::function<void(GUI::MouseEventArgs&)> mouseMove, std::function<void(GUI::MouseEventArgs&)> mouseDown, std::function<void(GUI::MouseEventArgs&)> mouseUp, std::function<void(GUI::MouseEventArgs&)> mouseScroll, std::function<bool(GUI::KeyEventArgs&)> keyPress)
			{
				mWindow->setNeedKeyFocus(true);
				needMouse();
				mKeyHandlers[id].push_back(keyPress);
				registerEvent(id, mouseMove, mWindow->eventMouseMove);
				registerEvent(id, mouseMove, mWindow->eventMouseDrag.m_event);
				registerEvent(id, mouseDown, mWindow->eventMouseButtonPressed);
				registerEvent(id, mouseUp, mWindow->eventMouseButtonReleased);
				registerEvent(id, mouseMove, mWindow->eventMouseWheel);
			}

			void MyGUIWindow::unregisterCustomEvents(void * id)
			{
				mEventHandlers.erase(id);
				mKeyHandlers.erase(id);
			}

			void MyGUIWindow::handleKeyEvent(MyGUI::Widget * w, MyGUI::KeyCode c)
			{
				KeyEventArgs args((Key)c.getValue());
				for (const std::pair<void *const, std::list<std::function<bool(GUI::KeyEventArgs&)>>> &p : mKeyHandlers) {
					for (const std::function<bool(GUI::KeyEventArgs&)> &f : p.second) {
						if (f(args))
							return;
					}
				}

				if (mWindow->getParent())
					mWindow->getParent()->eventKeyButtonReleased(w, c);
			}

			Window *MyGUIWindow::createImpl(Class _class)
			{
				Window *impl;
				switch (_class) {
				case Class::WINDOW_CLASS:
					impl = new Window(this);
					break;
				case Class::BAR_CLASS:
					impl = new MyGUIBar(this);
					break;
				case Class::BUTTON_CLASS:
					impl = new MyGUIButton(this);
					break;
				case Class::CHECKBOX_CLASS:
					impl = new MyGUICheckbox(this);
					break;
				case Class::COMBOBOX_CLASS:
					impl = new MyGUICombobox(this);
					break;
				case Class::LABEL_CLASS:
					impl = new MyGUILabel(this);
					break;
					/*case Class::TABWINDOW_CLASS:
					impl = new MyGUIT(this);
					break;*/
				case Class::TEXTBOX_CLASS:
					impl = new MyGUITextbox(this);
					break;
				case Class::TEXTUREDRAWER_CLASS:
					impl = new MyGUITextureDrawer(this);
					break;
				default:
					throw 0;
				}
				return impl;
			}

			void MyGUIWindow::showModal()
			{
				mWindow->detachFromWidget("Popup"); // DIRTY
				mWindow->setVisible(true);
				MyGUI::InputManager::getInstance().addWidgetModal(mWindow);
			}

			bool MyGUIWindow::isVisible()
			{
				return mWindow->getVisible();
			}

			void MyGUIWindow::releaseInput()
			{
			}

			void MyGUIWindow::captureInput()
			{
			}

			void MyGUIWindow::hideModal()
			{
				mWindow->setVisible(false);
				MyGUI::InputManager::getInstance().removeWidgetModal(mWindow);
			}

			void MyGUIWindow::show()
			{
				mWindow->setVisible(true);
			}

			void MyGUIWindow::hide()
			{
				mWindow->setVisible(false);
			}

			void MyGUIWindow::activate()
			{
				MyGUI::InputManager::getInstance().setKeyFocusWidget(mWindow);
			}

			void MyGUIWindow::moveToFront()
			{
				int depth = mWindow->getDepth();
				mWindow->setDepth(depth + 1);
				mWindow->setDepth(depth);
			}

			void MyGUIWindow::setEnabled(bool b)
			{
				mWindow->setEnabled(b);
			}


			void MyGUIWindow::registerEvent(void *id, EventType type, std::function<void()> event)
			{
				switch (type) {
				case EventType::ButtonClick:
					needMouse();
					registerEvent(id, event, mWindow->castType<MyGUI::Button>()->eventMouseButtonClick);
					break;
				case EventType::TextChanged:
					registerEvent(id, event, mWindow->castType<MyGUI::EditBox>()->eventEditTextChange.m_eventObsolete);
					break;
				case EventType::CloseClicked:
					registerEvent(id, [=](const std::string &name) {
						if (name == "close")
							event();
					}, mWindow->castType<MyGUI::Window>()->eventWindowButtonPressed.m_eventObsolete);
					break;
				case EventType::CheckboxToggled:
					registerEvent(id, event, mWindow->castType<MyGUI::Button>()->eventMouseButtonClick);
					break;
				case EventType::MouseEntered:
					needMouse();
					registerEvent(id, event, mWindow->eventMouseSetFocus);
					break;
				case EventType::MouseLeft:
					needMouse();
					registerEvent(id, event, mWindow->eventMouseLostFocus);
					break;
					/*case EventType::ComboboxSelectionChanged:
					//str = CEGUI::Combobox::EventListSelectionAccepted;
					break;*/
				default:
					WindowContainer::registerEvent(id, type, event);
				}

			}


			void MyGUIWindow::registerEvent(void *id, std::function<void(const std::string&)> f, MyGUI::EventHandle_WidgetString &event)
			{
				mEventHandlers[id].emplace_back(new EventHandler<MyGUI::Widget*, const std::string&>([=](MyGUI::Widget*, const std::string &name) {
					f(name);
				}, event));
			}

			void MyGUIWindow::registerEvent(void *id, std::function<void()> f, MyGUI::EventHandle_WidgetVoid &event)
			{
				mEventHandlers[id].emplace_back(new EventHandler<MyGUI::Widget*>([=](MyGUI::Widget*) {
					f();
				}, event));
			}

			void MyGUIWindow::registerEvent(void *id, std::function<void(MouseEventArgs&)> f, MyGUI::EventHandle_WidgetIntInt &event) {
				mEventHandlers[id].emplace_back(new EventHandler<MyGUI::Widget*, int, int>([=](MyGUI::Widget* w, int left, int top) {
					MouseEventArgs args(mGui->widgetRelative(w, left, top), mGui->relativeMoveDelta(w), 0);
					f(args);
				}, event));
			}

			void MyGUIWindow::registerEvent(void *id, std::function<void(MouseEventArgs&)> f, MyGUI::EventHandle_WidgetInt &event) {
				mEventHandlers[id].emplace_back(new EventHandler<MyGUI::Widget*, int>([=](MyGUI::Widget* w, int wheel) {
					MouseEventArgs args(mGui->widgetRelative(w), mGui->relativeMoveDelta(w), (float)wheel);
					f(args);
				}, event));
			}

			void MyGUIWindow::registerEvent(void *id, std::function<void()> f, MyGUI::EventHandle_WidgetWidget &event)
			{
				mEventHandlers[id].emplace_back(new EventHandler<MyGUI::Widget*, MyGUI::Widget*>([=](MyGUI::Widget*, MyGUI::Widget*) {
					f();
				}, event));
			}

			void MyGUIWindow::registerEvent(void *id, std::function<void(MouseEventArgs&)> f, MyGUI::EventHandle_WidgetIntIntButton &event) {
				mEventHandlers[id].emplace_back(new EventHandler<MyGUI::Widget*, int, int, MyGUI::MouseButton>([=](MyGUI::Widget* w, int left, int top, MyGUI::MouseButton button) {
					MouseEventArgs args(mGui->widgetRelative(w, left, top), mGui->relativeMoveDelta(w), 0, MyGUILauncher::convertButton(button));
					f(args);
				}, event));
			}


			WindowContainer * MyGUIWindow::loadLayoutWindow(const std::string & name)
			{
				bool failed = false;
				MyGUI::VectorWidgetPtr widgets;
				try {
					widgets = MyGUI::LayoutManager::getInstance().loadLayout(name, "", mWindow);
					if (widgets.size() != 1)
						failed = true;
				}
				catch (Ogre::FileNotFoundException &) {
					failed = true;
				}

				if (failed) {
					LOG_ERROR(Database::Exceptions::loadLayoutFailure(name));
					return 0;
				}
				MyGUI::Widget *w = widgets.front();
				w->setVisible(false);
				WindowContainer *result = new MyGUIWindow(w, mGui, this);
				result->buildHierarchy();
				return result;
			}

			std::list<WindowContainer*> MyGUIWindow::buildChildren()
			{
				std::list<WindowContainer*> result;
				MyGUI::Widget *w = mWindow->getClientWidget() ? mWindow->getClientWidget() : mWindow;
				for (size_t i = 0; i < w->getChildCount(); ++i) {
					result.push_back(new MyGUIWindow(w->getChildAt(i), mGui, this));
				}
				return result;
			}

			void MyGUIWindow::needMouse()
			{
				MyGUI::Widget *w = mWindow;
				while (w && !w->getNeedMouseFocus()) {
					w->setNeedMouseFocus(true);
					w = w->getParent();
				}
			}

			WindowContainer * MyGUIWindow::createChildWindow(const std::string & name, Class _class, const std::string &customSkin)
			{
				std::string type, skin;
				switch (_class) {
				case Class::WINDOW_CLASS:
					type = "Widget";
					skin = "PanelEmpty";
					break;
				case Class::TEXTUREDRAWER_CLASS:
					type = "ImageBox";
					skin = "ImageBox";
					break;
				case Class::BUTTON_CLASS:
					type = "Button";
					skin = "Button";
					break;
				case Class::BAR_CLASS:
					type = "ProgressBar";
					skin = "ProgressBarFill";
					break;
				default:
					throw 0;
				}
				if (!customSkin.empty())
					skin = customSkin;

				MyGUI::Widget *w = mWindow->createWidgetRealT(type, skin, { 0, 0, 1, 1 }, MyGUI::Align::Default, name);

				return new MyGUIWindow(w, mGui, this);
			}


			void MyGUIWindow::setPixelSize(const Ogre::Vector2 & size)
			{
				mWindow->setSize((int)size.x, (int)size.y);
			}

			void MyGUIWindow::setPixelPosition(const Ogre::Vector2 & pos)
			{
				mWindow->setPosition((int)pos.x, (int)pos.y);
			}

			Ogre::Vector2 MyGUIWindow::getPixelPosition()
			{
				const MyGUI::IntPoint &pos = (mWindow->getClientWidget() ? mWindow->getClientWidget() : mWindow)->getPosition();
				return{ (float)pos.left, (float)pos.top };
			}

			Ogre::Vector2 MyGUIWindow::getPixelSize()
			{
				const MyGUI::IntSize &size = (mWindow->getClientWidget() ? mWindow->getClientWidget() : mWindow)->getSize();
				return{ (float)size.width, (float)size.height };
			}



			MyGUI::Widget *MyGUIWindow::window() {
				return mWindow;
			}

		}
	}
}