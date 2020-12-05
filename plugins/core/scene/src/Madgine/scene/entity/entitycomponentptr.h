#pragma once

#include "entitycomponenthandle.h"
#include "entitycomponentlist.h"

#include "Modules/keyvalue/scopebase.h"

//TODO
#include "../scenemanager.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T>
        struct EntityComponentPtrBase;

        template <>
        struct MADGINE_SCENE_EXPORT EntityComponentPtrBase<EntityComponentBase> : ScopeBase {
            EntityComponentPtrBase();

            EntityComponentPtrBase(EntityComponentHandle<EntityComponentBase> data, size_t index, SceneManager *sceneMgr);
            EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other);

            EntityComponentPtrBase(EntityComponentPtrBase<EntityComponentBase> &&other);

            ~EntityComponentPtrBase();

            const EntityComponentHandle<EntityComponentBase> &handle() const;

            explicit operator EntityComponentHandle<EntityComponentBase>() const;

            /*const EntityPtr &entity() const
            {
                return mEntity;
            }*/

            SceneManager *sceneMgr() const;

            size_t index() const;

            EntityComponentBase *get() const;

            EntityComponentBase *operator->() const
            {
                return get();
            }

            operator EntityComponentBase *() const;

            operator bool() const;

            std::string_view name() const;

            TypedScopePtr getTyped() const;

        protected:
            EntityComponentHandle<EntityComponentBase> mHandle;
            size_t mIndex;
            SceneManager *mSceneMgr;
        };

        template <>
        struct MADGINE_SCENE_EXPORT EntityComponentPtrBase<const EntityComponentBase> : ScopeBase {
            EntityComponentPtrBase();

            EntityComponentPtrBase(EntityComponentHandle<const EntityComponentBase> data, size_t index, SceneManager *sceneMgr);

            EntityComponentPtrBase(const EntityComponentPtrBase<const EntityComponentBase> &other);

            EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other);

            ~EntityComponentPtrBase();

            const EntityComponentHandle<const EntityComponentBase> &handle() const
            {
                return mHandle;
            }

            explicit operator EntityComponentHandle<const EntityComponentBase>() const;

            /*const EntityPtr &entity() const
            {
                return mEntity;
            }*/

            SceneManager *sceneMgr() const
            {
                return mSceneMgr;
            }

            size_t index() const
            {
                return mIndex;
            }

        protected:
            EntityComponentHandle<const EntityComponentBase> mHandle;
            size_t mIndex;
            SceneManager *mSceneMgr;
        };

        template <typename T>
        struct EntityComponentPtrBase : ScopeBase {
            EntityComponentPtrBase() = default;

            EntityComponentPtrBase(const EntityComponentPtrBase<T> &other)
                : mHandle { other.sceneMgr()->template entityComponentList<T>()->copy(other.mHandle.mIndex) }
                , mSceneMgr(other.mSceneMgr)
            {
            }

            explicit EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other)
                : mHandle { other.handle().mIndex ? other.sceneMgr()->entityComponentList(other.index())->copy(other.handle().mIndex) : GenerationContainerIndex {} }
                , mSceneMgr(other.sceneMgr())
            {
                assert(!other || component_index<T>() == other.index());
            }

            template <typename U, typename = std::enable_if_t<std::is_convertible_v<T *, U *>>>
            explicit EntityComponentPtrBase(const EntityComponentPtrBase<U> &other)
                : mHandle { other.sceneMgr()->template entityComponentList<U>()->copy(other.handle().mIndex) }
                , mSceneMgr(other.sceneMgr())
            {
            }

            EntityComponentPtrBase(EntityComponentHandle<T> &&data, SceneManager *sceneMgr)
                : mHandle(std::move(data))
                , mSceneMgr(sceneMgr)
            {
            }

            EntityComponentPtrBase(const EntityComponentHandle<T> &data, SceneManager *sceneMgr)
                : mHandle { sceneMgr->template entityComponentList<T>()->copy(data.mIndex) }
                , mSceneMgr(sceneMgr)
            {
            }

            ~EntityComponentPtrBase()
            {
                if (mHandle.mIndex) {
                    mSceneMgr->template entityComponentList<T>()->reset(mHandle.mIndex);
                }
            }

            EntityComponentPtrBase<T> &operator=(const EntityComponentPtrBase<T> &other)
            {
                if (mHandle.mIndex)
                    mSceneMgr->template entityComponentList<T>()->reset(mHandle.mIndex);
                if (other.mHandle.mIndex)
                    mHandle.mIndex = other.mSceneMgr->template entityComponentList<T>()->copy(other.mHandle.mIndex);
                mSceneMgr = other.mSceneMgr;
                return *this;
            }

            EntityComponentPtrBase<T> &operator=(EntityComponentPtrBase<T> &&other)
            {
                mHandle = std::move(other.mHandle);
                std::swap(mSceneMgr, other.mSceneMgr);
                return *this;
            }

            const EntityComponentHandle<T> &handle() const
            {
                return mHandle;
            }

            explicit operator EntityComponentHandle<T>() const
            {
                return { mHandle.mIndex ? mSceneMgr->template entityComponentList<T>()->copy(mHandle.mIndex) : GenerationContainerIndex {} };
            }

            bool operator!=(const EntityComponentHandle<T> &handle) const
            {
                if (mSceneMgr) {
                    mSceneMgr->template entityComponentList<T>()->update(mHandle.mIndex);
                    mSceneMgr->template entityComponentList<T>()->update(handle.mIndex);
                }
                return mHandle != handle;
            }

            bool operator==(const EntityComponentHandle<T> &handle) const
            {
                if (mSceneMgr) {
                    mSceneMgr->template entityComponentList<T>()->update(mHandle.mIndex);
                    mSceneMgr->template entityComponentList<T>()->update(handle.mIndex);
                }
                return mHandle == handle;
            }

            /*const EntityPtr &entity() const
            {
                return mEntity;
            }*/

            SceneManager *sceneMgr() const
            {
                return mSceneMgr;
            }

            T* get() const {
                return mHandle.mIndex ? mSceneMgr->template entityComponentList<T>().get(mHandle.mIndex) : nullptr;
            }

            T *operator->() const
            {
                return get();
            }

            operator T *() const
            {
                return get();
            }

            void update()
            {
                mSceneMgr->template entityComponentList<T>()->update(mHandle.mIndex);
            }

        protected:
            EntityComponentHandle<T> mHandle;
            SceneManager *mSceneMgr;
        };

        template <typename T>
        struct EntityComponentPtr : EntityComponentPtrBase<T> {
            using EntityComponentPtrBase<T>::EntityComponentPtrBase;
        };

    }
}
}