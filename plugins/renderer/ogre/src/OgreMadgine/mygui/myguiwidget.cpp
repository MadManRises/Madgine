#include "../ogrelib.h"

#include "myguiwidget.h"

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

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIWidget::MyGUIWidget(const std::string& name, MyGUIWidget* parent) :
			Widget(name, parent),
		mScale(1.0f)
		{
		}

		MyGUIWidget::MyGUIWidget(const std::string& name, MyGUITopLevelWindow& window) :
			Widget(name, window),
		mScale(1.0f)
		{
		}

		Vector3 MyGUIWidget::getAbsoluteSize() const
		{
			return { static_cast<float>(widget()->getWidth()), static_cast<float>(widget()->getHeight()), mScale };
		}

		Vector2 MyGUIWidget::getAbsolutePosition() const
		{
			return { static_cast<float>(widget()->getLeft()), static_cast<float>(widget()->getTop()) };
		}

		void MyGUIWidget::releaseInput()
		{
			
		}

		void MyGUIWidget::captureInput()
		{
		}

		void MyGUIWidget::activate()
		{
			MyGUI::InputManager::getInstance().setKeyFocusWidget(widget());
		}

		void MyGUIWidget::moveToFront()
		{
			int depth = widget()->getDepth();
			widget()->setDepth(depth + 1);
			widget()->setDepth(depth);
		}

		bool MyGUIWidget::isVisible() const
		{
			return widget()->isVisible();
		}

		void MyGUIWidget::showModal()
		{
			widget()->detachFromWidget("Popup"); // DIRTY
			show();
			MyGUI::InputManager::getInstance().addWidgetModal(widget());
		}

		void MyGUIWidget::hideModal()
		{
			hide();
			MyGUI::InputManager::getInstance().removeWidgetModal(widget());
		}

		void MyGUIWidget::setVisible(bool b)
		{
			widget()->setVisible(b);
		}

		void MyGUIWidget::setEnabled(bool b)
		{
			widget()->setEnabled(b);
		}

		std::unique_ptr<Widget> MyGUIWidget::createWidget(const std::string& name)
		{
			return std::make_unique<MyGUIEmptyWidget>(name, this);
		}

		std::unique_ptr<Bar> MyGUIWidget::createBar(const std::string& name)
		{
			return std::make_unique<MyGUIBar>(name, this);
		}

		std::unique_ptr<Button> MyGUIWidget::createButton(const std::string& name)
		{
			return std::make_unique<MyGUIButton>(name, this);
		}

		std::unique_ptr<Checkbox> MyGUIWidget::createCheckbox(const std::string& name)
		{
			return std::make_unique<MyGUICheckbox>(name, this);
		}

		std::unique_ptr<Combobox> MyGUIWidget::createCombobox(const std::string& name)
		{
			return std::make_unique<MyGUICombobox>(name, this);
		}

		std::unique_ptr<Image> MyGUIWidget::createImage(const std::string& name)
		{
			return std::make_unique<MyGUIImage>(name, this);
		}

		std::unique_ptr<Label> MyGUIWidget::createLabel(const std::string& name)
		{
			return std::make_unique<MyGUILabel>(name, this);
		}

		std::unique_ptr<SceneWindow> MyGUIWidget::createSceneWindow(const std::string& name)
		{
			return std::make_unique<MyGUISceneWindow>(name, this);
		}

		std::unique_ptr<TabWidget> MyGUIWidget::createTabWidget(const std::string& name)
		{
			return std::make_unique<MyGUITabWidget>(name, this);
		}

		std::unique_ptr<Textbox> MyGUIWidget::createTextbox(const std::string& name)
		{
			return std::make_unique<MyGUITextbox>(name, this);
		}

		void MyGUIWidget::setAbsoluteSize(const Vector3& size)
		{
			widget()->setSize(size.x, size.y);
			mScale = size.z;
		}

		void MyGUIWidget::setAbsolutePosition(const Vector2& pos)
		{
			widget()->setPosition(pos.x, pos.y);
		}

		void MyGUIWidget::setup()
		{
			widget()->setNeedMouseFocus(true);
			widget()->eventMouseMove += MyGUI::newDelegate(this, &MyGUIWidget::emitMouseMove);
			widget()->eventMouseWheel += MyGUI::newDelegate(this, &MyGUIWidget::emitMouseWheel);
			widget()->eventMouseDrag += MyGUI::newDelegate(this, &MyGUIWidget::emitMouseMove);
			widget()->eventMouseButtonPressed += MyGUI::newDelegate(this, &MyGUIWidget::emitMouseDown);
			widget()->eventMouseButtonReleased += MyGUI::newDelegate(this, &MyGUIWidget::emitMouseUp);
			
		}

		void MyGUIWidget::emitMouseMove(MyGUI::Widget* w, int left, int top)
		{
			Input::MouseEventArgs args{ static_cast<MyGUITopLevelWindow&>(window()).widgetRelative(w, left, top), static_cast<MyGUITopLevelWindow&>(window()).relativeMoveDelta(w), 0 };
			mMouseMoveSignal.emit(args);
		}

		void MyGUIWidget::emitMouseWheel(MyGUI::Widget* w, int wheel)
		{
			Input::MouseEventArgs args{ static_cast<MyGUITopLevelWindow&>(window()).widgetRelative(w), static_cast<MyGUITopLevelWindow&>(window()).relativeMoveDelta(w), static_cast<float>(wheel) };
			mMouseMoveSignal.emit(args);
		}

		void MyGUIWidget::emitMouseDown(MyGUI::Widget* w, int left, int top, MyGUI::MouseButton button)
		{
			Input::MouseEventArgs args{ static_cast<MyGUITopLevelWindow&>(window()).widgetRelative(w, left, top), static_cast<MyGUITopLevelWindow&>(window()).relativeMoveDelta(w), 0, MyGUISystem::convertButton(button) };
			mMouseDownSignal.emit(args);
		}

		void MyGUIWidget::emitMouseUp(MyGUI::Widget* w, int left, int top, MyGUI::MouseButton button)
		{
			Input::MouseEventArgs args{ static_cast<MyGUITopLevelWindow&>(window()).widgetRelative(w, left, top), static_cast<MyGUITopLevelWindow&>(window()).relativeMoveDelta(w), 0, MyGUISystem::convertButton(button) };
			mMouseUpSignal.emit(args);
		}
	}
}
