#include "../clientlib.h"

#include "guisystem.h"
#include "Madgine/app/application.h"

#include "../ui/uimanager.h"

#include "widgets/toplevelwindow.h"

#include "widgets/widget.h"

#include "Interfaces/generic/keyvalueiterate.h"

#include "Interfaces/window/windowapi.h"

#include "Interfaces/debug/profiler/profiler.h"

//TODO Make uniform
#if UNIX
template <> DLL_EXPORT Engine::App::GlobalAPICollector::ComponentRegistrator<Engine::GUI::GUISystem> Engine::UniqueComponent<Engine::GUI::GUISystem, Engine::App::GlobalAPICollector>::_reg;
#endif

namespace Engine
{



	namespace GUI
	{
		
		GUISystem::GUISystem(App::Application &app) :
			App::GlobalAPI<GUISystem>(app),			
            mUI(std::make_unique<UI::UIManager>(*this)),
			mRenderer(this)
		{
		}


		GUISystem::~GUISystem()
		{

			mUI.reset();

			mWindows.clear();
			assert(mWidgets.empty());
		}

		

		bool GUISystem::init()
		{
			app(false).addFrameListener(this);

			markInitialized();
			
			if (!mRenderer->callInit() && mUI->callInit())
				return false;
			
			createTopLevelWindow();

			return true;
		}

		void GUISystem::finalize()
		{
			app(false).removeFrameListener(this);

			mUI->callFinalize();
			mRenderer->callFinalize();
			clear();
		}

		
		void GUISystem::clear()
		{
			mWindows.clear();
		}


		Widget* GUISystem::getWidget(const std::string& name)
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


	}
}
