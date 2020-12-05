#include "../../scenelib.h"

#include "entitycomponentptr.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "../scenemanager.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

using EntityComponentBasePtr = Engine::Scene::Entity::EntityComponentPtr<Engine::Scene::Entity::EntityComponentBase>;

METATABLE_BEGIN(EntityComponentBasePtr);
READONLY_PROPERTY(Get, getTyped)
METATABLE_END(EntityComponentBasePtr);

namespace Engine {

namespace Scene {
    namespace Entity {

        EntityComponentPtrBase<EntityComponentBase>::EntityComponentPtrBase() = default;

        EntityComponentPtrBase<EntityComponentBase>::EntityComponentPtrBase(EntityComponentHandle<EntityComponentBase> data, size_t index, SceneManager *sceneMgr)
            : mHandle(std::move(data))
            , mIndex(index)
            , mSceneMgr(sceneMgr)
        {
        }

        EntityComponentPtrBase<EntityComponentBase>::EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other)
            : mHandle { other.mSceneMgr->entityComponentList(other.mIndex)->copy(other.mHandle.mIndex) }
            , mIndex(other.mIndex)
            , mSceneMgr(other.mSceneMgr)
        {
        }

        EntityComponentPtrBase<EntityComponentBase>::EntityComponentPtrBase(EntityComponentPtrBase<EntityComponentBase> &&other)
            : mHandle(std::move(other.mHandle))
            , mIndex(other.mIndex)
            , mSceneMgr(std::exchange(other.mSceneMgr, nullptr))
        {
        }

        EntityComponentPtrBase<EntityComponentBase>::~EntityComponentPtrBase()
        {
            if (mHandle.mIndex) {
                mSceneMgr->entityComponentList(mIndex)->reset(mHandle.mIndex);
            }
        }

        EntityComponentPtrBase<EntityComponentBase>::operator EntityComponentHandle<EntityComponentBase>() const
        {
            return { mHandle.mIndex ? mSceneMgr->entityComponentList(mIndex)->copy(mHandle.mIndex) : GenerationContainerIndex {} };
        }

        EntityComponentBase *EntityComponentPtrBase<EntityComponentBase>::get() const
        {
            return mSceneMgr->entityComponentList(mIndex).get(mHandle.mIndex);
        }

        SceneManager *EntityComponentPtrBase<EntityComponentBase>::sceneMgr() const
        {
            return mSceneMgr;
        }

        size_t EntityComponentPtrBase<EntityComponentBase>::index() const
        {
            return mIndex;
        }

        EntityComponentPtrBase<EntityComponentBase>::operator EntityComponentBase *() const
        {
            return get();
        }

        std::string_view EntityComponentPtrBase<EntityComponentBase>::name() const
        {
            return std::find_if(sComponentsByName().begin(), sComponentsByName().end(), [&](const std::pair<const std::string_view, IndexRef> &p) { return p.second == mIndex; })->first;                
        }

        TypedScopePtr EntityComponentPtrBase<EntityComponentBase>::getTyped() const
        {
            return mSceneMgr->entityComponentList(mIndex).getTyped(mHandle.mIndex);
        }

        const EntityComponentHandle<EntityComponentBase> &EntityComponentPtrBase<EntityComponentBase>::handle() const
        {
            return mHandle;
        }

        EntityComponentPtrBase<EntityComponentBase>::operator bool() const
        {
            return mHandle.mIndex;
        }

        EntityComponentPtrBase<const EntityComponentBase>::EntityComponentPtrBase() = default;

        EntityComponentPtrBase<const EntityComponentBase>::EntityComponentPtrBase(const EntityComponentPtrBase<const EntityComponentBase> &other)
            : mHandle { other.mSceneMgr->entityComponentList(other.mIndex)->copy(other.mHandle.mIndex) }
            , mIndex(other.mIndex)
            , mSceneMgr(other.mSceneMgr)
        {
        }

        EntityComponentPtrBase<const EntityComponentBase>::EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other)
            : mHandle { other.sceneMgr()->entityComponentList(other.index())->copy(other.handle().mIndex) }
            , mIndex(other.index())
            , mSceneMgr(other.sceneMgr())
        {
        }

        EntityComponentPtrBase<const EntityComponentBase>::EntityComponentPtrBase(EntityComponentHandle<const EntityComponentBase> data, size_t index, SceneManager *sceneMgr)
            : mHandle(std::move(data))
            , mIndex(index)
            , mSceneMgr(sceneMgr)
        {
        }

        EntityComponentPtrBase<const EntityComponentBase>::~EntityComponentPtrBase()
        {
            if (mHandle.mIndex) {
                mSceneMgr->entityComponentList(mIndex)->reset(mHandle.mIndex);
            }
        }

        EntityComponentPtrBase<const EntityComponentBase>::operator EntityComponentHandle<const EntityComponentBase>() const
        {
            return { mHandle.mIndex ? mSceneMgr->entityComponentList(mIndex)->copy(mHandle.mIndex) : GenerationContainerIndex {} };
        }

    }
}
}