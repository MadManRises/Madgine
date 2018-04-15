#include "../../baselib.h"

#include "globalapicomponentbase.h"

#include "globalscopebase.h"


namespace Engine
{
	namespace Scripting
	{
		GlobalAPIComponentBase::GlobalAPIComponentBase(GlobalScopeBase &global) :
		ScopeBase(global.createTable()),
		mGlobal(global)
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
			return mGlobal.getGlobalAPIComponent(i);
		}

		Scene::SceneComponentBase& GlobalAPIComponentBase::getSceneComponent(size_t i)
		{
			return mGlobal.getSceneComponent(i);
		}

		Scene::SceneManagerBase& GlobalAPIComponentBase::sceneMgr()
		{
			return mGlobal.sceneMgr();
		}

		GlobalScopeBase& GlobalAPIComponentBase::globalScope()
		{
			return mGlobal;
		}

		UI::GuiHandlerBase& GlobalAPIComponentBase::getGuiHandler(size_t i)
		{
			return mGlobal.getGuiHandler(i);
		}

		UI::GameHandlerBase& GlobalAPIComponentBase::getGameHandler(size_t i)
		{
			return mGlobal.getGameHandler(i);
		}
	}
}
