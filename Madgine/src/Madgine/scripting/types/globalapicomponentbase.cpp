#include "../../baselib.h"

#include "globalapicomponentbase.h"

#include "globalscopebase.h"

#include "../../app/application.h"


namespace Engine
{
	namespace Scripting
	{
		GlobalAPIComponentBase::GlobalAPIComponentBase(App::Application &app) :
		ScopeBase(app.createTable()),
		mApp(app)
		{
		}

		bool GlobalAPIComponentBase::init()
		{
			return MadgineObject::init();
		}

		void GlobalAPIComponentBase::finalize()
		{
			MadgineObject::finalize();
		}

		void GlobalAPIComponentBase::clear()
		{
		}

		void GlobalAPIComponentBase::update()
		{
		}

		GlobalAPIComponentBase& GlobalAPIComponentBase::getGlobalAPIComponent(size_t i)
		{
			return mApp.getGlobalAPIComponent(i);
		}

		Scene::SceneComponentBase& GlobalAPIComponentBase::getSceneComponent(size_t i)
		{
			return mApp.getSceneComponent(i);
		}

		Scene::SceneManager& GlobalAPIComponentBase::sceneMgr()
		{
			return mApp.sceneMgr();
		}

		GlobalScopeBase& GlobalAPIComponentBase::globalScope()
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
