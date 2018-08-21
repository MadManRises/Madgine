#include "../../clientlib.h"

#include "widget.h"

#include "toplevelwindow.h"

#include "../widgetnames.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwidget.h"
#include "textbox.h"

#include "../../app/clientapplication.h"
#include "../guisystem.h"


namespace Engine
{

	API_IMPL(GUI::Widget, MAP_RO(AbsolutePos, getAbsolutePosition), MAP_RO(AbsoluteSize, getAbsoluteSize), MAP(Visible, isVisible, setVisible));

	namespace GUI
	{
		
		Widget::Widget(const std::string& name, Widget* parent) :
			Scope(parent),
			mName(name),
			mParent(parent),
			mWindow(parent->window()),
			mPos(Matrix3::ZERO),
			mSize(Matrix3::IDENTITY)
		{
			gui().registerWidget(this);
		}

		Widget::Widget(const std::string &name, TopLevelWindow &window) :
			Scope(&window),
			mName(name),
			mParent(nullptr),
			mWindow(window),
			mPos(Matrix3::ZERO),
			mSize(Matrix3::IDENTITY)
		{
			gui().registerWidget(this);
		}

		Widget::~Widget()
		{
			gui().unregisterWidget(this);
		}

		void Widget::setSize(const Matrix3& size)
		{
			mSize = size;
			if (mParent)
				updateGeometry(mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
			else {
				updateGeometry(mWindow.getScreenSize(), { 0.0f,0.0f });
			}
		}

		const Matrix3& Widget::getSize()
		{
			return mSize;
		}

		void Widget::setPos(const Matrix3& pos)
		{
			mPos = pos;
			updateGeometry(mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
		}

		const Matrix3& Widget::getPos() const
		{
			return mPos;
		}

		void Widget::updateGeometry(const Vector3& parentSize, const Vector2& parentPos)
		{
			setAbsoluteSize(mSize * parentSize);
			setAbsolutePosition((mPos * parentSize).xy() + parentPos);

			for (const std::unique_ptr<Widget> &child : mChildren)
			{
				child->updateGeometry(getAbsoluteSize(), getAbsolutePosition());
			}
		}

		void Widget::destroy()
		{
			if (mParent)
				mParent->destroyChild(this);
			else
				mWindow.destroyTopLevel(this);
		}

		const std::string &Widget::getName() const
		{
			return mName;
		}

		const char *Widget::key() const
		{
			return mName.c_str();
		}

		Widget* Widget::createChildWidget(const std::string& name)
		{
			Widget *w = mChildren.emplace_back(createWidget(name)).get();
			w->updateGeometry(getAbsoluteSize(), { 0.0f,0.0f });
			return w;
		}
		
		Bar* Widget::createChildBar(const std::string& name)
		{
			std::unique_ptr<Bar> p = createBar(name);
			Bar *b = p.get();
			mChildren.emplace_back(std::move(p));
			b->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return b;
		}

		Button* Widget::createChildButton(const std::string& name)
		{
			std::unique_ptr<Button> p = createButton(name);
			Button *b = p.get();
			mChildren.emplace_back(std::move(p));
			b->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return b;
		}

		Checkbox* Widget::createChildCheckbox(const std::string& name)
		{
			std::unique_ptr<Checkbox> p = createCheckbox(name);
			Checkbox *c = p.get();
			mChildren.emplace_back(std::move(p));
			c->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return c;
		}

		Combobox* Widget::createChildCombobox(const std::string& name)
		{
			std::unique_ptr<Combobox> p = createCombobox(name);
			Combobox *c = p.get();
			mChildren.emplace_back(std::move(p));
			c->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return c;
		}

		Label* Widget::createChildLabel(const std::string& name)
		{
			std::unique_ptr<Label> p = createLabel(name);
			Label *l = p.get();
			mChildren.emplace_back(std::move(p));
			l->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return l;
		}

		SceneWindow* Widget::createChildSceneWindow(const std::string& name)
		{
			std::unique_ptr<SceneWindow> p = createSceneWindow(name);
			SceneWindow *s = p.get();
			mChildren.emplace_back(std::move(p));
			s->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return s;
		}

		TabWidget* Widget::createChildTabWidget(const std::string& name)
		{
			std::unique_ptr<TabWidget> p = createTabWidget(name);
			TabWidget *t = p.get();
			mChildren.emplace_back(std::move(p));
			t->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return t;
		}

		Textbox* Widget::createChildTextbox(const std::string& name)
		{
			std::unique_ptr<Textbox> p = createTextbox(name);
			Textbox *t = p.get();
			mChildren.emplace_back(std::move(p));
			t->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return t;
		}

		Image* Widget::createChildImage(const std::string& name)
		{
			std::unique_ptr<Image> p = createImage(name);
			Image *i = p.get();
			mChildren.emplace_back(std::move(p));
			i->updateGeometry(getAbsoluteSize(), { 0.0f, 0.0f });
			return i;
		}

		GUISystem& Widget::gui()
		{
			return mWindow.gui();
		}

		TopLevelWindow& Widget::window()
		{
			return mWindow;
		}

		void Widget::destroyChild(Widget* w)
		{
			auto it = std::find_if(mChildren.begin(), mChildren.end(), [=](const std::unique_ptr<Widget> &ptr) {return ptr.get() == w; });
			assert(it != mChildren.end());
			mChildren.erase(it);
		}

		KeyValueMapList Widget::maps()
		{
			return Scope::maps().merge(mChildren);
		}

		void Widget::show()
		{
			setVisible(true);
		}

		void Widget::hide()
		{
			setVisible(false);
		}

		Widget* Widget::getChildRecursive(const std::string& name)
		{
			if (name == getName() || name == WidgetNames::thisWidget) return this;
			for (const std::unique_ptr<Widget> &w : mChildren)
				if (Widget* f = w->getChildRecursive(name))
					return f;
			return nullptr;
		}

		SignalSlot::SignalStub<Input::MouseEventArgs&>& Widget::mouseMoveEvent()
		{
			return mMouseMoveSignal;
		}

		SignalSlot::SignalStub<Input::MouseEventArgs&>& Widget::mouseDownEvent()
		{
			return mMouseDownSignal;
		}

		SignalSlot::SignalStub<Input::MouseEventArgs&>& Widget::mouseUpEvent()
		{
			return mMouseUpSignal;
		}

		std::unique_ptr<Widget> Widget::createWidgetClass(const std::string& name, Class _class)
		{
			switch (_class)
			{
			case Class::BAR_CLASS:
				return createBar(name);
			case Class::CHECKBOX_CLASS:
				return createCheckbox(name);
			case Class::LABEL_CLASS:
				return createLabel(name);
			case Class::TABWINDOW_CLASS:
				return createTabWidget(name);
			case Class::BUTTON_CLASS:
				return createButton(name);
			case Class::COMBOBOX_CLASS:
				return createCombobox(name);
			case Class::TEXTBOX_CLASS:
				return createTextbox(name);
			case Class::SCENEWINDOW_CLASS:
				return createSceneWindow(name);
			case Class::IMAGE_CLASS:
				return createImage(name);
			default:
				throw 0;
			}
		}
	}
}
