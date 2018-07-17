#include "../../clientlib.h"

#include "window.h"

#include "../guisystem.h"

#include "../windownames.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwindow.h"
#include "textbox.h"

namespace Engine
{
	namespace GUI
	{
		
		Window::Window(const std::string& name, Window* parent) :
			mName(name),
			mParent(parent),
			mGUI(parent->gui()),
			mPos(Matrix3::ZERO),
			mSize(Matrix3::IDENTITY)
		{
			mGUI.registerWindow(this);
		}

		Window::Window(const std::string &name, GUISystem &gui) :
			mName(name),
			mParent(nullptr),
			mGUI(gui),
			mPos(Matrix3::ZERO),
			mSize(Matrix3::IDENTITY)
		{
			mGUI.registerWindow(this);
		}

		Window::~Window()
		{
			mGUI.unregisterWindow(this);
		}

		void Window::setSize(const Matrix3& size)
		{
			mSize = size;
		}

		const Matrix3& Window::getSize()
		{
			return mSize;
		}

		void Window::setPos(const Matrix3& pos)
		{
			mPos = pos;
		}

		const Matrix3& Window::getPos() const
		{
			return mPos;
		}

		void Window::updateGeometry(const Vector3& parentSize, const Vector2& parentPos)
		{
			setAbsoluteSize(mSize * parentSize);
			setAbsolutePosition((mPos * parentSize).xy() + parentPos);

			for (const std::unique_ptr<Window> &child : mChildren)
			{
				child->updateGeometry(getAbsoluteSize(), getAbsolutePosition());
			}
		}

		void Window::destroy()
		{
			if (mParent)
				mParent->destroyChild(this);
			else
				mGUI.destroyTopLevel(this);
		}

		const std::string &Window::getName() const
		{
			return mName;
		}

		Window* Window::createChildWindow(const std::string& name)
		{
			return mChildren.emplace_back(createWindow(name)).get();
		}
		
		Bar* Window::createChildBar(const std::string& name)
		{
			std::unique_ptr<Bar> p = createBar(name);
			Bar *b = p.get();
			mChildren.emplace_back(std::move(p));
			return b;
		}

		Button* Window::createChildButton(const std::string& name)
		{
			std::unique_ptr<Button> p = createButton(name);
			Button *b = p.get();
			mChildren.emplace_back(std::move(p));
			return b;
		}

		Checkbox* Window::createChildCheckbox(const std::string& name)
		{
			std::unique_ptr<Checkbox> p = createCheckbox(name);
			Checkbox *c = p.get();
			mChildren.emplace_back(std::move(p));
			return c;
		}

		Combobox* Window::createChildCombobox(const std::string& name)
		{
			std::unique_ptr<Combobox> p = createCombobox(name);
			Combobox *c = p.get();
			mChildren.emplace_back(std::move(p));
			return c;
		}

		Label* Window::createChildLabel(const std::string& name)
		{
			std::unique_ptr<Label> p = createLabel(name);
			Label *l = p.get();
			mChildren.emplace_back(std::move(p));
			return l;
		}

		SceneWindow* Window::createChildSceneWindow(const std::string& name)
		{
			std::unique_ptr<SceneWindow> p = createSceneWindow(name);
			SceneWindow *s = p.get();
			mChildren.emplace_back(std::move(p));
			return s;
		}

		TabWindow* Window::createChildTabWindow(const std::string& name)
		{
			std::unique_ptr<TabWindow> p = createTabWindow(name);
			TabWindow *t = p.get();
			mChildren.emplace_back(std::move(p));
			return t;
		}

		Textbox* Window::createChildTextbox(const std::string& name)
		{
			std::unique_ptr<Textbox> p = createTextbox(name);
			Textbox *t = p.get();
			mChildren.emplace_back(std::move(p));
			return t;
		}

		Image* Window::createChildImage(const std::string& name)
		{
			std::unique_ptr<Image> p = createImage(name);
			Image *i = p.get();
			mChildren.emplace_back(std::move(p));
			return i;
		}

		GUISystem& Window::gui()
		{
			return mGUI;
		}

		void Window::destroyChild(Window* w)
		{
			auto it = std::find_if(mChildren.begin(), mChildren.end(), [=](const std::unique_ptr<Window> &ptr) {return ptr.get() == w; });
			assert(it != mChildren.end());
			mChildren.erase(it);
		}

		void Window::show()
		{
			setVisible(true);
		}

		void Window::hide()
		{
			setVisible(false);
		}

		Window* Window::getChildRecursive(const std::string& name)
		{
			if (name == getName() || name == WindowNames::thisWindow) return this;
			for (const std::unique_ptr<Window> &w : mChildren)
				if (Window* f = w->getChildRecursive(name))
					return f;
			return nullptr;
		}

		std::unique_ptr<Window> Window::createWindowClass(const std::string& name, Class _class)
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
				return createTabWindow(name);
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