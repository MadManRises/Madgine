#include "baselib.h"
#include "scenecomponentbase.h"
#include "scripting/types/api.h"
#include "generic/keyvalueiterate.h"

API_IMPL(Engine::Scene::SceneComponentBase, MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId), MAP_RO(Active, isActive));

namespace Engine
{


	namespace Scene
	{
		SceneComponentBase::SceneComponentBase(SceneManagerBase* sceneMgr, ContextMask context) :
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
			return MadgineObject::init();
		}

		void SceneComponentBase::finalize()
		{
			MadgineObject::finalize();
		}

		void SceneComponentBase::setEnabled(bool b)
		{
			mEnabled = b;
		}

		bool SceneComponentBase::isEnabled() const
		{
			return mEnabled;
		}

		SceneManagerBase* SceneComponentBase::sceneMgr() const
		{
			return mSceneMgr;
		}

		void SceneComponentBase::update(float)
		{
		}

		void SceneComponentBase::fixedUpdate(float)
		{
		}

		KeyValueMapList SceneComponentBase::maps()
		{
			return ScopeBase::maps().merge(Scripting::API<SceneComponentBase>::api());
		}
	}
}
