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
            mUI(std::make_unique<UI::UIManager>(*this))
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
			markInitialized();
			return mUI->callInit();
		}

		void GUISystem::finalize()
		{
			mUI->callFinalize();
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
			return Scope::maps().merge(mWindows);
		}

		const std::vector<std::unique_ptr<TopLevelWindow>> &GUISystem::topLevelWindows()
		{
			return mWindows;
		}

		void GUISystem::closeTopLevelWindow(TopLevelWindow * w)
		{
			mWindows.erase(std::find_if(mWindows.begin(), mWindows.end(), [w](const std::unique_ptr<TopLevelWindow> &p) {return p.get() == w; }));
		}

		bool GUISystem::sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame)
		{
			PROFILE();
			Window::sUpdate();
			bool result = false;
			for (const std::unique_ptr<TopLevelWindow> &w : mWindows)
				result |= w->update();
			return FrameLoop::sendFrameRenderingQueued(timeSinceLastFrame, mUI->currentContext()) && result;
		}

		void GUISystem::addTopLevelWindow(std::unique_ptr<TopLevelWindow>&& window)
		{
			mWindows.emplace_back(std::forward<std::unique_ptr<TopLevelWindow>>(window));
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

		bool GUISystem::singleFrame(std::chrono::microseconds timeSinceLastFrame)
		{
			return sendFrameStarted(timeSinceLastFrame) && sendFrameRenderingQueued(timeSinceLastFrame) && sendFrameEnded(timeSinceLastFrame);
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

		App::GlobalAPIComponentBase& GUISystem::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getGlobalAPIComponent(i, init);
		}

	}
}
