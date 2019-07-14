#include "../baselib.h"

#include "globalapibase.h"

#include "application.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

namespace Engine
{
	namespace App
	{
		GlobalAPIBase::GlobalAPIBase(App::Application &app) :
		mApp(app)
		{
		}

		GlobalAPIBase& GlobalAPIBase::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		App::Application& GlobalAPIBase::app(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return mApp.getSelf(init);
		}

		const Core::MadgineObject * GlobalAPIBase::parent() const
		{
			return &mApp;
		}

		bool GlobalAPIBase::init()
		{
			return true;
		}

		void GlobalAPIBase::finalize()
		{
		}

		void GlobalAPIBase::update()
		{
		}

		GlobalAPIBase& GlobalAPIBase::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getGlobalAPIComponent(i, init);
		}

		Scene::SceneComponentBase& GlobalAPIBase::getSceneComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getSceneComponent(i, init);
		}

		Scene::SceneManager& GlobalAPIBase::sceneMgr(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.sceneMgr(init);
		}

		Scripting::GlobalScopeBase& GlobalAPIBase::globalScope()
		{
			return mApp;
		}

	}
}

METATABLE_BEGIN(Engine::App::GlobalAPIBase)
METATABLE_END(Engine::App::GlobalAPIBase)

RegisterType(Engine::App::GlobalAPIBase);