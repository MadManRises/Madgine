#include "../baselib.h"
#include "scenecomponentbase.h"
#include "Interfaces/scripting/types/api.h"
#include "Interfaces/generic/keyvalueiterate.h"
#include "scenemanager.h"

//API_IMPL(Engine::Scene::SceneComponentBase);


namespace Engine
{


	namespace Scene
	{
		SceneComponentBase::SceneComponentBase(SceneManager &sceneMgr, ContextMask context) :
			ScopeBase(&sceneMgr),
			mContext(context),
			mEnabled(true),
			mSceneMgr(sceneMgr)
		{
		}

		void SceneComponentBase::update(float timeSinceLastFrame, ContextMask mask)
		{
			if (mEnabled && mContext & (mask | ContextMask::AnyContext))
			{
				update(timeSinceLastFrame);
			}
		}

		void SceneComponentBase::fixedUpdate(float timeStep, ContextMask mask)
		{
			if (mEnabled && mContext & (mask | ContextMask::AnyContext))
			{
				fixedUpdate(timeStep);
			}
		}

		bool SceneComponentBase::init()
		{
			return true;
		}

		void SceneComponentBase::finalize()
		{			
		}

		void SceneComponentBase::setEnabled(bool b)
		{
			mEnabled = b;
		}

		bool SceneComponentBase::isEnabled() const
		{
			return mEnabled;
		}

		SceneManager &SceneComponentBase::sceneMgr(bool init) const
		{
			if (init)
			{
				checkInitState();
			}
			return mSceneMgr.getSelf(init);
		}

		SceneComponentBase& SceneComponentBase::getSceneComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mSceneMgr.getComponent(i, init);
		}

		App::GlobalAPIComponentBase &SceneComponentBase::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mSceneMgr.getGlobalAPIComponent(i, init);
		}

		SceneComponentBase& SceneComponentBase::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		void SceneComponentBase::update(float)
		{
		}

		void SceneComponentBase::fixedUpdate(float)
		{
		}

		KeyValueMapList SceneComponentBase::maps()
		{
			return ScopeBase::maps().merge(MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId), MAP_RO(Active, isActive));
		}
	}
}
