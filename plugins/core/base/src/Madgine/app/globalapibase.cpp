#include "../baselib.h"

#include "globalapibase.h"

#include "application.h"

#include "Modules/keyvalue/metatable_impl.h"

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

		const MadgineObject * GlobalAPIBase::parent() const
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

		GlobalAPIBase& GlobalAPIBase::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mApp.getGlobalAPIComponent(i, init);
		}

	}
}

METATABLE_BEGIN(Engine::App::GlobalAPIBase)
METATABLE_END(Engine::App::GlobalAPIBase)

