#include "../scenelib.h"
#include "scenecomponentbase.h"
#include "scenemanager.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"


namespace Engine
{


	namespace Scene
	{
    SceneComponentBase::SceneComponentBase(SceneManager &sceneMgr):
			mSceneMgr(sceneMgr)
		{
		}

		bool SceneComponentBase::init()
		{
			return true;
		}

		void SceneComponentBase::finalize()
		{			
		}

		SceneManager &SceneComponentBase::sceneMgr(bool init) const
		{
			if (init)
			{
				checkInitState();
			}
			return mSceneMgr.getSelf(init);
                }

                std::string_view SceneComponentBase::key() const
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

		void SceneComponentBase::update(std::chrono::microseconds)
		{
		}

	}
}


METATABLE_BEGIN(Engine::Scene::SceneComponentBase)
METATABLE_END(Engine::Scene::SceneComponentBase)


SERIALIZETABLE_BEGIN(Engine::Scene::SceneComponentBase)
SERIALIZETABLE_END(Engine::Scene::SceneComponentBase)

