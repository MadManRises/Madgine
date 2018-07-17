#include "../clientlib.h"

#include "guisystem.h"
#include "../app/application.h"

#include "../ui/uimanager.h"

#include "windows/bar.h"
#include "windows/button.h"
#include "windows/checkbox.h"
#include "windows/combobox.h"
#include "windows/label.h"
#include "windows/scenewindow.h"
#include "windows/tabwindow.h"
#include "windows/textbox.h"
#include "windows/image.h"

namespace Engine
{

	API_IMPL(GUI::GUISystem);

	namespace GUI
	{
		
		GUISystem::GUISystem(App::Application &app) :
			Scripting::Scope<GUISystem>(app.createTable()),			
			mApp(app)
		{
		}


		GUISystem::~GUISystem()
		{
			mUI.reset();

			mTopLevelWindows.clear();
			assert(mWindows.empty());
		}

		void GUISystem::showCursor()
		{
			setCursorVisibility(true);
		}

		void GUISystem::hideCursor()
		{
			setCursorVisibility(false);
		}

		bool GUISystem::preInit()
		{
			mUI = std::make_unique<UI::UIManager>(*this);
			return mUI->preInit();
		}

		bool GUISystem::init()
		{
			return MadgineObject::init() && mUI->init();
		}

		void GUISystem::finalize()
		{
			mUI->finalize();
			MadgineObject::finalize();
		}

		Window* GUISystem::getWindowByName(const std::string& name)
		{
			return mWindows.at(name);
		}

		Window* GUISystem::createTopLevelWindow(const std::string& name)
		{
			Window *w = mTopLevelWindows.emplace_back(createWindow(name)).get();
			w->hide();
			return w;
		}

		Bar* GUISystem::createTopLevelBar(const std::string& name)
		{
			std::unique_ptr<Bar> p = createBar(name);
			Bar *b = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			return b;
		}

		Button* GUISystem::createTopLevelButton(const std::string& name)
		{
			std::unique_ptr<Button> p = createButton(name);
			Button *b = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			b->hide();
			return b;
		}

		Checkbox* GUISystem::createTopLevelCheckbox(const std::string& name)
		{
			std::unique_ptr<Checkbox> p = createCheckbox(name);
			Checkbox *c = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			c->hide();
			return c;
		}

		Combobox* GUISystem::createTopLevelCombobox(const std::string& name)
		{
			std::unique_ptr<Combobox> p = createCombobox(name);
			Combobox *c = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			c->hide();
			return c;
		}

		Image* GUISystem::createTopLevelImage(const std::string& name)
		{
			std::unique_ptr<Image> p = createImage(name);
			Image *i = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			i->hide();
			return i;
		}

		Label* GUISystem::createTopLevelLabel(const std::string& name)
		{
			std::unique_ptr<Label> p = createLabel(name);
			Label *l = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			l->hide();
			return l;
		}

		SceneWindow* GUISystem::createTopLevelSceneWindow(const std::string& name)
		{
			std::unique_ptr<SceneWindow> p = createSceneWindow(name);
			SceneWindow *s = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			s->hide();
			return s;
		}

		TabWindow* GUISystem::createTopLevelTabWindow(const std::string& name)
		{
			std::unique_ptr<TabWindow> p = createTabWindow(name);
			TabWindow *t = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			t->hide();
			return t;
		}

		Textbox* GUISystem::createTopLevelTextbox(const std::string& name)
		{
			std::unique_ptr<Textbox> p = createTextbox(name);
			Textbox *t = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			t->hide();
			return t;
		}

		std::unique_ptr<Window> GUISystem::createWindowClass(const std::string& name, Class _class)
		{
			switch(_class)
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

		void GUISystem::calculateWindowGeometries()
		{
			Vector2 size = getScreenSize();
			for (const std::unique_ptr<Window> &topLevel : mTopLevelWindows)
			{				
				topLevel->updateGeometry({ size.x, size.y, 1.0f }, { 0.0f, 0.0f });
			}
		}

		void GUISystem::registerWindow(Window* w)
		{
			assert(mWindows.try_emplace(w->getName(), w).second);
		}

		void GUISystem::unregisterWindow(Window* w)
		{
			assert(mWindows.erase(w->getName()) == 1);
		}

		void GUISystem::destroyTopLevel(Window* w)
		{
			auto it = std::find_if(mTopLevelWindows.begin(), mTopLevelWindows.end(), [=](const std::unique_ptr<Window> &ptr) {return ptr.get() == w; });
			assert(it != mTopLevelWindows.end());
			mTopLevelWindows.erase(it);
		}

		bool GUISystem::sendFrameRenderingQueued(float timeSinceLastFrame)
		{
			return FrameLoop::sendFrameRenderingQueued(timeSinceLastFrame, mUI->currentContext());
		}

		Scene::SceneManager& GUISystem::sceneMgr()
		{
			return mApp.sceneMgr();
		}

		UI::UIManager& GUISystem::ui()
		{
			return *mUI;
		}

		App::Application &GUISystem::app()
		{
			return mApp;
		}

		Scene::SceneComponentBase& GUISystem::getSceneComponent(size_t i)
		{
			return mApp.getSceneComponent(i);
		}

		Scripting::GlobalAPIComponentBase& GUISystem::getGlobalAPIComponent(size_t i)
		{
			return mApp.getGlobalAPIComponent(i);
		}

	}
}