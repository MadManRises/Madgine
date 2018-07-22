#include "../ogrelib.h"

#include "myguiwindow.h"

#include "myguisystem.h"

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

namespace Engine
{
	namespace GUI
	{
		MyGUIWindow::MyGUIWindow(const std::string& name, MyGUIWindow* parent) :
		Window(name, parent),
		mScale(1.0f)
		{
		}

		MyGUIWindow::MyGUIWindow(const std::string& name, MyGUISystem& gui) :
		Window(name, gui),
		mScale(1.0f)
		{
		}

		Vector3 MyGUIWindow::getAbsoluteSize() const
		{
			return { (float)widget()->getWidth(), (float)widget()->getHeight(), mScale };
		}

		Vector2 MyGUIWindow::getAbsolutePosition() const
		{
			return { (float)widget()->getLeft(), (float)widget()->getTop() };
		}

		void MyGUIWindow::releaseInput()
		{
			
		}

		void MyGUIWindow::captureInput()
		{
		}

		void MyGUIWindow::activate()
		{
			MyGUI::InputManager::getInstance().setKeyFocusWidget(widget());
		}

		void MyGUIWindow::moveToFront()
		{
			int depth = widget()->getDepth();
			widget()->setDepth(depth + 1);
			widget()->setDepth(depth);
		}

		bool MyGUIWindow::isVisible() const
		{
			return widget()->isVisible();
		}

		void MyGUIWindow::showModal()
		{
			widget()->detachFromWidget("Popup"); // DIRTY
			show();
			MyGUI::InputManager::getInstance().addWidgetModal(widget());
		}

		void MyGUIWindow::hideModal()
		{
			hide();
			MyGUI::InputManager::getInstance().removeWidgetModal(widget());
		}

		void MyGUIWindow::setVisible(bool b)
		{
			widget()->setVisible(b);
		}

		void MyGUIWindow::setEnabled(bool b)
		{
			widget()->setEnabled(b);
		}

		std::unique_ptr<Window> MyGUIWindow::createWindow(const std::string& name)
		{
			return std::make_unique<MyGUIEmptyWindow>(name, this);
		}

		std::unique_ptr<Bar> MyGUIWindow::createBar(const std::string& name)
		{
			return std::make_unique<MyGUIBar>(name, this);
		}

		std::unique_ptr<Button> MyGUIWindow::createButton(const std::string& name)
		{
			return std::make_unique<MyGUIButton>(name, this);
		}

		std::unique_ptr<Checkbox> MyGUIWindow::createCheckbox(const std::string& name)
		{
			return std::make_unique<MyGUICheckbox>(name, this);
		}

		std::unique_ptr<Combobox> MyGUIWindow::createCombobox(const std::string& name)
		{
			return std::make_unique<MyGUICombobox>(name, this);
		}

		std::unique_ptr<Image> MyGUIWindow::createImage(const std::string& name)
		{
			return std::make_unique<MyGUIImage>(name, this);
		}

		std::unique_ptr<Label> MyGUIWindow::createLabel(const std::string& name)
		{
			return std::make_unique<MyGUILabel>(name, this);
		}

		std::unique_ptr<SceneWindow> MyGUIWindow::createSceneWindow(const std::string& name)
		{
			return std::make_unique<MyGUISceneWindow>(name, this);
		}

		std::unique_ptr<TabWindow> MyGUIWindow::createTabWindow(const std::string& name)
		{
			return std::make_unique<MyGUITabWindow>(name, this);
		}

		std::unique_ptr<Textbox> MyGUIWindow::createTextbox(const std::string& name)
		{
			return std::make_unique<MyGUITextbox>(name, this);
		}

		void MyGUIWindow::setAbsoluteSize(const Vector3& size)
		{
			widget()->setSize(size.x, size.y);
			mScale = size.z;
		}

		void MyGUIWindow::setAbsolutePosition(const Vector2& pos)
		{
			widget()->setPosition(pos.x, pos.y);
		}

		void MyGUIWindow::setup()
		{
			widget()->setNeedMouseFocus(true);
			widget()->eventMouseMove += MyGUI::newDelegate(this, &MyGUIWindow::emitMouseMove);
			widget()->eventMouseWheel += MyGUI::newDelegate(this, &MyGUIWindow::emitMouseWheel);
			widget()->eventMouseDrag += MyGUI::newDelegate(this, &MyGUIWindow::emitMouseMove);
			widget()->eventMouseButtonPressed += MyGUI::newDelegate(this, &MyGUIWindow::emitMouseDown);
			widget()->eventMouseButtonReleased += MyGUI::newDelegate(this, &MyGUIWindow::emitMouseUp);
			
		}

		void MyGUIWindow::emitMouseMove(MyGUI::Widget* w, int left, int top)
		{
			MouseEventArgs args{ static_cast<MyGUISystem&>(gui()).widgetRelative(w, left, top), static_cast<MyGUISystem&>(gui()).relativeMoveDelta(w), 0 };
			mMouseMoveSignal.emit(args);
		}

		void MyGUIWindow::emitMouseWheel(MyGUI::Widget* w, int wheel)
		{
			MouseEventArgs args{ static_cast<MyGUISystem&>(gui()).widgetRelative(w), static_cast<MyGUISystem&>(gui()).relativeMoveDelta(w), static_cast<float>(wheel) };
			mMouseMoveSignal.emit(args);
		}

		void MyGUIWindow::emitMouseDown(MyGUI::Widget* w, int left, int top, MyGUI::MouseButton button)
		{
			MouseEventArgs args{ static_cast<MyGUISystem&>(gui()).widgetRelative(w, left, top), static_cast<MyGUISystem&>(gui()).relativeMoveDelta(w), 0, MyGUISystem::convertButton(button) };
			mMouseDownSignal.emit(args);
		}

		void MyGUIWindow::emitMouseUp(MyGUI::Widget* w, int left, int top, MyGUI::MouseButton button)
		{
			MouseEventArgs args{ static_cast<MyGUISystem&>(gui()).widgetRelative(w, left, top), static_cast<MyGUISystem&>(gui()).relativeMoveDelta(w), 0, MyGUISystem::convertButton(button) };
			mMouseUpSignal.emit(args);
		}
	}
}
