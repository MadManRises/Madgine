#include "../baselib.h"

#include "globalapicomponentbase.h"

#include "application.h"

namespace Engine
{
	namespace App
	{
		GlobalAPIComponentBase::GlobalAPIComponentBase(App::Application &app) :
		ScopeBase(&app),
		mApp(app)
		{
		}

		GlobalAPIComponentBase& GlobalAPIComponentBase::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		App::Application& GlobalAPIComponentBase::app(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return mApp.getSelf(init);
		}

		bool GlobalAPIComponentBase::init()
		{
			return true;
		}

		void GlobalAPIComponentBase::finalize()
		{
		}

		void GlobalAPIComponentBase::update()
		{
		}

		GlobalAPIComponentBase& GlobalAPIComponentBase::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getGlobalAPIComponent(i, init);
		}

		Scene::SceneComponentBase& GlobalAPIComponentBase::getSceneComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getSceneComponent(i, init);
		}

		Scene::SceneManager& GlobalAPIComponentBase::sceneMgr(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.sceneMgr(init);
		}

		Scripting::GlobalScopeBase& GlobalAPIComponentBase::globalScope()
		{
			return mApp;
		}

		/*UI::GuiHandlerBase& GlobalAPIComponentBase::getGuiHandler(size_t i)
		{
			return mApp.getGuiHandler(i);
		}

		UI::GameHandlerBase& GlobalAPIComponentBase::getGameHandler(size_t i)
		{
			return mApp.getGameHandler(i);
		}*/
	}
}

