#include "../../clientlib.h"

#include "toplevelwindow.h"


#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwidget.h"
#include "textbox.h"
#include "image.h"

#include "../guisystem.h"

#include "../../input/inputhandler.h"

namespace Engine
{
	API_IMPL(GUI::TopLevelWindow);

	namespace GUI
	{
		TopLevelWindow::TopLevelWindow(GUISystem& gui) :
			Scope(&gui),
			mGui(gui),
		mInput(nullptr)
		{
		}

		TopLevelWindow::~TopLevelWindow()
		{
			mTopLevelWidgets.clear();
			
		}

		bool TopLevelWindow::update()
		{
			try
			{
				//PROFILE("Input");
				if (mInput)
					mInput->update();
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("Unhandled Exception during Input!");
				LOG_EXCEPTION(e);
			}

			return true;
		}

		void TopLevelWindow::showCursor()
		{
			setCursorVisibility(true);
		}

		void TopLevelWindow::hideCursor()
		{
			setCursorVisibility(false);
		}


		Widget* TopLevelWindow::createTopLevelWidget(const std::string& name)
		{
			Widget *w = mTopLevelWidgets.emplace_back(createWidget(name)).get();
			w->hide();
			w->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return w;
		}

		Bar* TopLevelWindow::createTopLevelBar(const std::string& name)
		{
			std::unique_ptr<Bar> p = createBar(name);
			Bar *b = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			b->hide();
			b->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return b;
		}

		Button* TopLevelWindow::createTopLevelButton(const std::string& name)
		{
			std::unique_ptr<Button> p = createButton(name);
			Button *b = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			b->hide();
			b->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return b;
		}

		Checkbox* TopLevelWindow::createTopLevelCheckbox(const std::string& name)
		{
			std::unique_ptr<Checkbox> p = createCheckbox(name);
			Checkbox *c = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			c->hide();
			c->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return c;
		}

		Combobox* TopLevelWindow::createTopLevelCombobox(const std::string& name)
		{
			std::unique_ptr<Combobox> p = createCombobox(name);
			Combobox *c = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			c->hide();
			c->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return c;
		}

		Image* TopLevelWindow::createTopLevelImage(const std::string& name)
		{
			std::unique_ptr<Image> p = createImage(name);
			Image *i = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			i->hide();
			i->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return i;
		}

		Label* TopLevelWindow::createTopLevelLabel(const std::string& name)
		{
			std::unique_ptr<Label> p = createLabel(name);
			Label *l = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			l->hide();
			l->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return l;
		}

		SceneWindow* TopLevelWindow::createTopLevelSceneWindow(const std::string& name)
		{
			std::unique_ptr<SceneWindow> p = createSceneWindow(name);
			SceneWindow *s = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			s->hide();
			s->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return s;
		}

		TabWidget* TopLevelWindow::createTopLevelTabWidget(const std::string& name)
		{
			std::unique_ptr<TabWidget> p = createTabWidget(name);
			TabWidget *t = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			t->hide();
			t->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return t;
		}

		Textbox* TopLevelWindow::createTopLevelTextbox(const std::string& name)
		{
			std::unique_ptr<Textbox> p = createTextbox(name);
			Textbox *t = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			t->hide();
			t->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return t;
		}

		std::unique_ptr<Widget> TopLevelWindow::createWidgetClass(const std::string& name, Class _class)
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

		KeyValueMapList TopLevelWindow::maps()
		{
			return Scope::maps().merge(mTopLevelWidgets);
		}

		Input::InputHandler* TopLevelWindow::input()
		{
			return mInput;
		}

		void TopLevelWindow::setInput(Input::InputHandler* input)
		{
			mInput = input;
			mInput->setListener(this);
		}

		void TopLevelWindow::calculateWindowGeometries()
		{
			Vector3 size = getScreenSize();
			for (const std::unique_ptr<Widget> &topLevel : mTopLevelWidgets)
			{
				topLevel->updateGeometry(size, { 0.0f, 0.0f });
			}
		}


		void TopLevelWindow::destroyTopLevel(Widget* w)
		{
			auto it = std::find_if(mTopLevelWidgets.begin(), mTopLevelWidgets.end(), [=](const std::unique_ptr<Widget> &ptr) {return ptr.get() == w; });
			assert(it != mTopLevelWidgets.end());
			mTopLevelWidgets.erase(it);
		}

		void TopLevelWindow::clear()
		{
			mTopLevelWidgets.clear();
		}

		GUISystem& TopLevelWindow::gui()
		{
			return mGui;
		}
	}
}