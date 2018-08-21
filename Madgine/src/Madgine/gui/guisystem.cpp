#include "../clientlib.h"

#include "guisystem.h"
#include "../app/clientapplication.h"

#include "../ui/uimanager.h"

#include "widgets/toplevelwindow.h"

#include "widgets/widget.h"

namespace Engine
{

	API_IMPL(GUI::GUISystem);

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
			if (!w->getName().empty())
				assert(mWidgets.try_emplace(w->getName(), w).second);
		}

		void GUISystem::unregisterWidget(Widget* w)
		{
			if (!w->getName().empty())
				assert(mWidgets.erase(w->getName()) == 1);
		}


		KeyValueMapList GUISystem::maps()
		{
			return Scope::maps().merge(mWindows);
		}

		bool GUISystem::singleFrame()
		{
			bool result = !mWindows.empty();
			for (const std::unique_ptr<TopLevelWindow> &w : mWindows)
				result |= w->singleFrame();
			return result;
		}

		const std::vector<std::unique_ptr<TopLevelWindow>> &GUISystem::topLevelWindows()
		{
			return mWindows;
		}

		bool GUISystem::sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame)
		{
			bool result = !mWindows.empty();
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

		Scripting::GlobalAPIComponentBase& GUISystem::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getGlobalAPIComponent(i, init);
		}

	}
}