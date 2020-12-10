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

            EntityComponentPtrBase(EntityComponentHandle<EntityComponentBase> data, SceneManager *sceneMgr);
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

            uint32_t type() const;

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
            SceneManager *mSceneMgr;
        };

        template <>
        struct MADGINE_SCENE_EXPORT EntityComponentPtrBase<const EntityComponentBase> : ScopeBase {
            EntityComponentPtrBase();

            EntityComponentPtrBase(EntityComponentHandle<const EntityComponentBase> data, SceneManager *sceneMgr);

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

            size_t type() const
            {
                return mHandle.mType;
            }

        protected:
            EntityComponentHandle<const EntityComponentBase> mHandle;
            SceneManager *mSceneMgr;
        };

        template <typename T>
        struct EntityComponentPtrBase : ScopeBase {
            EntityComponentPtrBase() = default;

            EntityComponentPtrBase(const EntityComponentPtrBase<T> &other)
                : mHandle { other.mHandle }
                , mSceneMgr(other.mSceneMgr)
            {
            }

            explicit EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other)
                : mHandle { other.handle()}
                , mSceneMgr(other.sceneMgr())
            {
                assert(!other || component_index<T>() == other.type());
            }

            template <typename U, typename = std::enable_if_t<std::is_convertible_v<T *, U *>>>
            explicit EntityComponentPtrBase(const EntityComponentPtrBase<U> &other)
                : mHandle { other.mHandle }
                , mSceneMgr(other.sceneMgr())
            {
            }

            EntityComponentPtrBase(EntityComponentHandle<T> data, SceneManager *sceneMgr)
                : mHandle(data)
                , mSceneMgr(sceneMgr)
            {
            }

            ~EntityComponentPtrBase()
            {
            }

            EntityComponentPtrBase<T> &operator=(const EntityComponentPtrBase<T> &other)
            {
                mHandle = other.mHandle;
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
                return mHandle;
            }

            bool operator!=(const EntityComponentHandle<T> &handle) const
            {
                return mHandle != handle;
            }

            bool operator==(const EntityComponentHandle<T> &handle) const
            {
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
                return mHandle ? mSceneMgr->template entityComponentList<T>().get(mHandle) : nullptr;
            }

            T *operator->() const
            {
                return get();
            }

            operator T *() const
            {
                return get();
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