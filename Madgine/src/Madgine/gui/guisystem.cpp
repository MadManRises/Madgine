#include "../clientlib.h"

#include "guisystem.h"
#include "../app/clientapplication.h"

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
		
		GUISystem::GUISystem(App::ClientApplication &app) :
			Scripting::Scope<GUISystem>(app.createTable()),			
			mApp(app),
            mUI(std::make_unique<UI::UIManager>(*this))
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

		bool GUISystem::init()
		{
			markInitialized();
			return mUI->callInit();
		}

		void GUISystem::finalize()
		{
			mUI->callFinalize();
			clear();
		}

		Window* GUISystem::getWindowByName(const std::string& name)
		{
			return mWindows.at(name);
		}

		Window* GUISystem::createTopLevelWindow(const std::string& name)
		{
			Window *w = mTopLevelWindows.emplace_back(createWindow(name)).get();
			w->hide();
			w->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return w;
		}

		Bar* GUISystem::createTopLevelBar(const std::string& name)
		{
			std::unique_ptr<Bar> p = createBar(name);
			Bar *b = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			b->hide();
			b->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return b;
		}

		Button* GUISystem::createTopLevelButton(const std::string& name)
		{
			std::unique_ptr<Button> p = createButton(name);
			Button *b = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			b->hide();			
			b->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return b;
		}

		Checkbox* GUISystem::createTopLevelCheckbox(const std::string& name)
		{
			std::unique_ptr<Checkbox> p = createCheckbox(name);
			Checkbox *c = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			c->hide();
			c->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return c;
		}

		Combobox* GUISystem::createTopLevelCombobox(const std::string& name)
		{
			std::unique_ptr<Combobox> p = createCombobox(name);
			Combobox *c = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			c->hide();
			c->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return c;
		}

		Image* GUISystem::createTopLevelImage(const std::string& name)
		{
			std::unique_ptr<Image> p = createImage(name);
			Image *i = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			i->hide();
			i->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return i;
		}

		Label* GUISystem::createTopLevelLabel(const std::string& name)
		{
			std::unique_ptr<Label> p = createLabel(name);
			Label *l = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			l->hide();
			l->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return l;
		}

		SceneWindow* GUISystem::createTopLevelSceneWindow(const std::string& name)
		{
			std::unique_ptr<SceneWindow> p = createSceneWindow(name);
			SceneWindow *s = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			s->hide();
			s->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return s;
		}

		TabWindow* GUISystem::createTopLevelTabWindow(const std::string& name)
		{
			std::unique_ptr<TabWindow> p = createTabWindow(name);
			TabWindow *t = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			t->hide();			
			t->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
			return t;
		}

		Textbox* GUISystem::createTopLevelTextbox(const std::string& name)
		{
			std::unique_ptr<Textbox> p = createTextbox(name);
			Textbox *t = p.get();
			mTopLevelWindows.emplace_back(std::move(p));
			t->hide();			
			t->updateGeometry(getScreenSize(), { 0.0f, 0.0f });
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
			Vector3 size = getScreenSize();
			for (const std::unique_ptr<Window> &topLevel : mTopLevelWindows)
			{				
				topLevel->updateGeometry(size, { 0.0f, 0.0f });
			}
		}

		void GUISystem::registerWindow(Window* w)
		{
			if (!w->getName().empty())
				assert(mWindows.try_emplace(w->getName(), w).second);
		}

		void GUISystem::unregisterWindow(Window* w)
		{
			if (!w->getName().empty())
				assert(mWindows.erase(w->getName()) == 1);
		}

		void GUISystem::destroyTopLevel(Window* w)
		{
			auto it = std::find_if(mTopLevelWindows.begin(), mTopLevelWindows.end(), [=](const std::unique_ptr<Window> &ptr) {return ptr.get() == w; });
			assert(it != mTopLevelWindows.end());
			mTopLevelWindows.erase(it);
		}

		void GUISystem::clear()
		{
			mTopLevelWindows.clear();
		}

		KeyValueMapList GUISystem::maps()
		{
			return Scope::maps().merge(mTopLevelWindows);
		}

		bool GUISystem::sendFrameRenderingQueued(float timeSinceLastFrame)
		{
			return FrameLoop::sendFrameRenderingQueued(timeSinceLastFrame, mUI->currentContext());
		}

		Scene::SceneManager& GUISystem::sceneMgr(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.sceneMgr(init);
		}

		UI::UIManager& GUISystem::ui(bool init)
		{
			if (init)
			{
				checkInitState();
				mUI->callInit();
			}
			return mUI->getSelf(init);
		}

		GUISystem& GUISystem::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		App::ClientApplication &GUISystem::app(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getSelf(init);
		}

		Scene::SceneComponentBase& GUISystem::getSceneComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getSceneComponent(i);
		}

		Scripting::GlobalAPIComponentBase& GUISystem::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getGlobalAPIComponent(i);
		}

	}
}