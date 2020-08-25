#pragma once

#include "../scenemanager.h"
#include "entitycomponenthandle.h"
#include "entitycomponentlist.h"
#include "entityptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T>
        struct EntityComponentPtrBase {
            EntityComponentPtrBase() = default;

            EntityComponentPtrBase(const EntityComponentPtrBase<T> &other)
                : mHandle { other.mEntity.sceneMgr()->template entityComponentList<T>()->copy(other.mHandle.mIndex) }
                , mEntity(other.mEntity)
            {
            }

            explicit EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other)
                : mHandle { other.handle().mIndex ? other.entity().sceneMgr()->entityComponentList(other.index())->copy(other.handle().mIndex) : GenerationVectorIndex {} }
                , mEntity(other.entity())
            {
                assert(!other || T::component_index() == other.index());
            }

            template <typename U, typename = std::enable_if_t<std::is_convertible_v<T *, U *>>>
            explicit EntityComponentPtrBase(const EntityComponentPtrBase<U> &other)
                : mHandle { other.entity().sceneMgr()->entityComponentList<U>()->copy(other.handle().mIndex) }
                , mEntity(other.entity())
            {
            }

            EntityComponentPtrBase(EntityComponentHandle<T> &&data, EntityPtr entity)
                : mHandle(std::move(data))
                , mEntity(std::move(entity))
            {
            }

            EntityComponentPtrBase(const EntityComponentHandle<T> &data, EntityPtr entity)
                : mHandle { entity.sceneMgr()->entityComponentList<T>()->copy(data.mIndex)}
                , mEntity(std::move(entity))
            {
            }

            ~EntityComponentPtrBase()
            {
                if (mHandle.mIndex) {
                    mEntity.sceneMgr()->entityComponentList<T>()->reset(mHandle.mIndex);
                }
            }

            EntityComponentPtrBase<T> &operator=(const EntityComponentPtrBase<T> &other)
            {
                mHandle = { other.mEntity.sceneMgr()->template entityComponentList<T>()->copy(other.mHandle.mIndex) };
                mEntity = other.mEntity;
                return *this;
            }

            EntityComponentPtrBase<T> &operator=(EntityComponentPtrBase<T> &&other)
            {
                mHandle = std::move(other.mHandle);
                mEntity = other.mEntity;
                return *this;
            }

            const EntityComponentHandle<T> &handle() const
            {
                return mHandle;
            }

            explicit operator EntityComponentHandle<T>() const
            {
                return { mHandle.mIndex ? mEntity.sceneMgr()->entityComponentList<T>()->copy(mHandle.mIndex) : GenerationVectorIndex {} };
            }

            bool operator!=(const EntityComponentHandle<T>& handle) const {
                if (mEntity) {
                    mEntity.sceneMgr()->entityComponentList<T>()->update(mHandle.mIndex);
                    mEntity.sceneMgr()->entityComponentList<T>()->update(handle.mIndex);
                }
                return mHandle != handle;
            }

            bool operator==(const EntityComponentHandle<T> &handle) const
            {
                if (mEntity) {
                    mEntity.sceneMgr()->entityComponentList<T>()->update(mHandle.mIndex);
                    mEntity.sceneMgr()->entityComponentList<T>()->update(handle.mIndex);
                }
                return mHandle == handle;
            }

            const EntityPtr &entity() const
            {
                return mEntity;
            }

            T *operator->() const
            {
                return mHandle.mIndex ? &mEntity.sceneMgr()->entityComponentList<T>()->at(mHandle.mIndex) : nullptr;
            }

            operator T *() const
            {
                return mHandle.mIndex ? &mEntity.sceneMgr()->entityComponentList<T>()->at(mHandle.mIndex) : nullptr;
            }

            void update() {
                mEntity.sceneMgr()->entityComponentList<T>()->update(mHandle.mIndex);
                mEntity.update();
            }

        protected:
            EntityComponentHandle<T> mHandle;
            EntityPtr mEntity;
        };

        template <>
        struct EntityComponentPtrBase<EntityComponentBase> {
            EntityComponentPtrBase() = default;

            EntityComponentPtrBase(EntityComponentHandle<EntityComponentBase> data, size_t index, EntityPtr entity)
                : mHandle(std::move(data))
                , mIndex(index)
                , mEntity(std::move(entity))
            {
            }
            EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other)
                : mHandle { other.entity().sceneMgr()->entityComponentList(other.mIndex)->copy(other.mHandle.mIndex) }
                , mIndex(other.mIndex)
                , mEntity(other.mEntity)
            {
            }

            EntityComponentPtrBase(EntityComponentPtrBase<EntityComponentBase> &&other)
                : mHandle(std::move(other.mHandle))
                , mIndex(other.mIndex)
                , mEntity(std::move(other.mEntity))
            {
            }

            ~EntityComponentPtrBase()
            {
                if (mHandle.mIndex) {
                    mEntity.sceneMgr()->entityComponentList(mIndex)->reset(mHandle.mIndex);
                }
            }

            const EntityComponentHandle<EntityComponentBase> &handle() const
            {
                return mHandle;
            }

            const EntityPtr &entity() const
            {
                return mEntity;
            }

            size_t index() const
            {
                return mIndex;
            }

            EntityComponentBase *get() const
            {
                return mEntity.sceneMgr()->entityComponentList(mIndex).get(mHandle.mIndex);
            }

            EntityComponentBase *operator->() const
            {
                return get();
            }

            operator bool() const
            {
                return mHandle.mIndex;
            }

        protected:
            EntityComponentHandle<EntityComponentBase> mHandle;
            size_t mIndex;
            EntityPtr mEntity;
        };

        template <>
        struct EntityComponentPtrBase<const EntityComponentBase> {
            EntityComponentPtrBase() = default;

            EntityComponentPtrBase(EntityComponentHandle<const EntityComponentBase> data, size_t index, EntityPtr entity)
                : mHandle(std::move(data))
                , mIndex(index)
                , mEntity(std::move(entity))
            {
            }

            EntityComponentPtrBase(const EntityComponentPtrBase<const EntityComponentBase> &other)
                : mHandle { other.entity().sceneMgr()->entityComponentList(other.mIndex)->copy(other.mHandle.mIndex) }
                , mIndex(other.mIndex)
                , mEntity(other.mEntity)
            {
            }

            EntityComponentPtrBase(const EntityComponentPtrBase<EntityComponentBase> &other)
                : mHandle { other.entity().sceneMgr()->entityComponentList(other.index())->copy(other.handle().mIndex) }
                , mIndex(other.index())
                , mEntity(other.entity())
            {
            }

            ~EntityComponentPtrBase()
            {
                if (mHandle.mIndex) {
                    mEntity.sceneMgr()->entityComponentList(mIndex)->reset(mHandle.mIndex);
                }
            }

            const EntityComponentHandle<const EntityComponentBase> &handle() const
            {
                return mHandle;
            }

            const EntityPtr &entity() const
            {
                return mEntity;
            }

            size_t index() const
            {
                return mIndex;
            }

        protected:
            EntityComponentHandle<const EntityComponentBase> mHandle;
            size_t mIndex;
            EntityPtr mEntity;
        };

        template <typename T>
        struct EntityComponentPtr : EntityComponentPtrBase<T> {
            using EntityComponentPtrBase<T>::EntityComponentPtrBase;
        };

    }
}
}