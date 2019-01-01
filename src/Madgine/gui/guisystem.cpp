#include "../clientlib.h"

#include "guisystem.h"
#include "../app/clientapplication.h"

#include "../ui/uimanager.h"

#include "widgets/toplevelwindow.h"

#include "widgets/widget.h"

#include "Interfaces/generic/keyvalueiterate.h"

#include "Interfaces/window/windowapi.h"

#include "Interfaces/debug/profiler/profiler.h"

namespace Engine
{



	namespace GUI
	{
		
		GUISystem::GUISystem(App::ClientApplication &app) :
			Scripting::Scope<GUISystem>(&app),			
			mApp(app),
            mUI(std::make_unique<UI::UIManager>(*this)),
			mRenderer(this)
		{
			app.addFrameListener(this);
		}


		GUISystem::~GUISystem()
		{
			mApp.removeFrameListener(this);

			mUI.reset();

			mWindows.clear();
			assert(mWidgets.empty());
		}

		

		bool GUISystem::init()
		{
			markInitialized();
			
			if (!mRenderer->callInit() && mUI->callInit())
				return false;
			
			createTopLevelWindow();

			return true;
		}

		void GUISystem::finalize()
		{
			mUI->callFinalize();
			mRenderer->callFinalize();
			clear();
		}

		
		void GUISystem::clear()
		{
			mWindows.clear();
		}


		Widget* GUISystem::getWidgetByName(const std::string& name)
		{
			return mWidgets.at(name);
		}

		void GUISystem::registerWidget(Widget* w)
		{
			if (!w->getName().empty()) {
				auto pib = mWidgets.try_emplace(w->getName(), w);
				assert(pib.second);
			}
		}

		void GUISystem::unregisterWidget(Widget* w)
		{
			if (!w->getName().empty()) {
				size_t result = mWidgets.erase(w->getName());
				assert(result == 1);
			}
		}


		KeyValueMapList GUISystem::maps()
		{
			return Scope::maps().merge(mWindows, mRenderer.get());
		}

		const std::vector<std::unique_ptr<TopLevelWindow>> &GUISystem::topLevelWindows()
		{
			return mWindows;
		}

		void GUISystem::closeTopLevelWindow(TopLevelWindow * w)
		{
			mWindows.erase(std::find_if(mWindows.begin(), mWindows.end(), [w](const std::unique_ptr<TopLevelWindow> &p) {return p.get() == w; }));
		}

		Render::RendererBase & GUISystem::renderer()
		{
			return *mRenderer;
		}


		TopLevelWindow *GUISystem::createTopLevelWindow()
		{
			return mWindows.emplace_back(std::make_unique<TopLevelWindow>(*this)).get();
		}

		bool GUISystem::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{
			PROFILE();
			Window::sUpdate();
			return !mWindows.empty();
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
			return mApp.getSceneComponent(i, init);
		}

		App::GlobalAPIBase& GUISystem::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getGlobalAPIComponent(i, init);
		}

	}
}
