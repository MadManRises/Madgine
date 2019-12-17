#include "../scenelib.h"
#include "scenecomponentbase.h"
#include "scenemanager.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"


namespace Engine
{


	namespace Scene
	{
    SceneComponentBase::SceneComponentBase(SceneManager &sceneMgr/*, Threading::ContextMask context*/):
			/*mContext(context),*/
			mEnabled(true),
			mSceneMgr(sceneMgr)
		{
		}

		/*void SceneComponentBase::update(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask mask)
		{
			if (mEnabled && mContext & (mask | ContextMask::AnyContext))
			{
				update(timeSinceLastFrame);
			}
		}

		void SceneComponentBase::fixedUpdate(std::chrono::microseconds timeStep, ContextMask mask)
		{
			if (mEnabled && mContext & (mask | ContextMask::AnyContext))
			{
				fixedUpdate(timeStep);
			}
		}*/

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

                const char *SceneComponentBase::key() const
                {
                    return serializeType()->mTypeName;
                }

		SceneComponentBase& SceneComponentBase::getSceneComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mSceneMgr.getComponent(i, init);
		}

		App::GlobalAPIBase &SceneComponentBase::getGlobalAPIComponent(size_t i, bool init)
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

		const MadgineObject * SceneComponentBase::parent() const
		{
			return &mSceneMgr;
		}

		/*App::Application & SceneComponentBase::app(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mSceneMgr.app(init);
		}*/

		void SceneComponentBase::update(std::chrono::microseconds)
		{
		}

		void SceneComponentBase::fixedUpdate(std::chrono::microseconds)
		{
		}

		/*KeyValueMapList SceneComponentBase::maps()
		{
			return ScopeBase::maps().merge(MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId), MAP_RO(Synced, isSynced));
		}*/
	}
}


METATABLE_BEGIN(Engine::Scene::SceneComponentBase)
METATABLE_END(Engine::Scene::SceneComponentBase)


SERIALIZETABLE_BEGIN(Engine::Scene::SceneComponentBase)
SERIALIZETABLE_END(Engine::Scene::SceneComponentBase)

RegisterType(Engine::Scene::SceneComponentBase);