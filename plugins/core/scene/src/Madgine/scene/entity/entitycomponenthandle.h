#pragma once

#include "Generic/callerhierarchy.h"

#include "Meta/serialize/streams/streamresult.h"

namespace Engine {
namespace Scene {

    namespace Entity {

        template <typename T>
        struct EntityComponentHandle;

        void entityComponentHelperWrite(Serialize::SerializeOutStream &out, const EntityComponentHandle<EntityComponentBase> &index, const char *name, const SceneManager *mgr);
        Serialize::StreamResult entityComponentHelperRead(Serialize::SerializeInStream &in, EntityComponentHandle<EntityComponentBase> &index, const char *name, SceneManager *mgr);

        void entityComponentOwningHelperWrite(Serialize::SerializeOutStream &out, const EntityComponentHandle<EntityComponentBase> &index, const char *name, CallerHierarchyBasePtr hierarchy);
        Serialize::StreamResult entityComponentOwningHelperRead(Serialize::SerializeInStream &in, const EntityComponentHandle<EntityComponentBase> &index, const char *name, CallerHierarchyBasePtr hierarchy);

        template <>
        struct EntityComponentHandle<EntityComponentBase> {
            IndexType<uint32_t> mIndex;
            IndexType<uint32_t> mType;

            Serialize::StreamResult readState(Serialize::SerializeInStream &in, const char *name, SceneManager *mgr)
            {
                return entityComponentHelperRead(in, *this, name, mgr);
            }

            void writeState(Serialize::SerializeOutStream &out, const char *name, SceneManager *mgr) const
            {
                entityComponentHelperWrite(out, *this, name, mgr);
            }

            bool operator!=(const EntityComponentHandle<EntityComponentBase> &other) const
            {
                assert(mType == other.mType);
                return mIndex != other.mIndex;
            }

            bool operator==(const EntityComponentHandle<EntityComponentBase> &other) const
            {
                assert(mType == other.mType);
                return mIndex == other.mIndex;
            }

            explicit operator bool() const
            {
                return bool(mIndex);
            }

            uint32_t type() const
            {
                return mType;
            }

            void relocate(const EntityComponentHandle<EntityComponentBase> &index)
            {
                assert(mType == index.mType);
                mIndex = index.mIndex;
            }
        };

        template <>
        struct EntityComponentHandle<const EntityComponentBase> {
            IndexType<uint32_t> mIndex;
            IndexType<uint32_t> mType;

            EntityComponentHandle() = default;

            EntityComponentHandle(EntityComponentHandle<EntityComponentBase> handle)
                : mIndex(handle.mIndex)
                , mType(handle.mType)
            {
            }

            bool operator!=(const EntityComponentHandle<EntityComponentBase> &other) const
            {
                assert(mType == other.mType);
                return mIndex != other.mIndex;
            }

            bool operator==(const EntityComponentHandle<EntityComponentBase> &other) const
            {
                assert(mType == other.mType);
                return mIndex == other.mIndex;
            }

            explicit operator bool() const
            {
                return bool(mIndex);
            }

            uint32_t type() const
            {
                return mType;
            }

            void relocate(const EntityComponentHandle<EntityComponentBase> &index)
            {
                assert(mType == index.mType);
                mIndex = index.mIndex;
            }
        };

        template <typename T>
        struct EntityComponentHandle {
            IndexType<uint32_t> mIndex;

            EntityComponentHandle() = default;

            EntityComponentHandle(const EntityComponentHandle<std::conditional_t<std::is_const_v<T>, const EntityComponentBase, EntityComponentBase>> &other)
                : mIndex(other.mIndex)
            {
                assert(!other || component_index<T>() == other.mType);
            }

            Serialize::StreamResult readState(Serialize::SerializeInStream &in, const char *name, SceneManager *mgr)
            {
                EntityComponentHandle<EntityComponentBase> handle { IndexType<uint32_t>::sInvalid, type() };
                STREAM_PROPAGATE_ERROR(entityComponentHelperRead(in, handle, name, mgr));
                mIndex = handle.mIndex;
                return {};
            }

            void writeState(Serialize::SerializeOutStream &out, const char *name, const SceneManager *mgr) const
            {
                entityComponentHelperWrite(out, *this, name, mgr);
            }

            bool operator!=(const EntityComponentHandle<T> &other) const
            {
                return mIndex != other.mIndex;
            }

            bool operator==(const EntityComponentHandle<T> &other) const
            {
                return mIndex == other.mIndex;
            }

            explicit operator bool() const
            {
                return bool(mIndex);
            }

            uint32_t type() const
            {
                return component_index<T>();
            }

            operator EntityComponentHandle<EntityComponentBase>() const
            {
                return { mIndex, type() };
            }

            void relocate(const EntityComponentHandle<EntityComponentBase> &index)
            {
                assert(index.mType == type());
                mIndex = index.mIndex;
            }
        };

        template <typename T>
        struct EntityComponentOwningHandle {
            EntityComponentHandle<T> mHandle;

            EntityComponentOwningHandle(EntityComponentHandle<T> handle)
                : mHandle(std::move(handle))
            {
            }
            EntityComponentOwningHandle(const EntityComponentOwningHandle<T> &other) = delete;
            EntityComponentOwningHandle(EntityComponentOwningHandle<T> &&other) = default;

            EntityComponentOwningHandle& operator=(const EntityComponentOwningHandle &) = delete;

            operator EntityComponentHandle<T>() const
            {
                return mHandle;
            }

            Serialize::StreamResult readState(Serialize::SerializeInStream &in, const char *name, CallerHierarchyBasePtr hierarchy)
            {
                return entityComponentOwningHelperRead(in, mHandle, name, hierarchy);
            }

            void writeState(Serialize::SerializeOutStream &out, const char *name, CallerHierarchyBasePtr hierarchy) const
            {
                entityComponentOwningHelperWrite(out, mHandle, name, hierarchy);
            }

            bool operator<(const EntityComponentOwningHandle<T> &other) const
            {
                return mHandle.type() < other.mHandle.type();
            }

            bool operator<(uint32_t type) const
            {
                return mHandle.type() < type;
            }

            void relocate(const EntityComponentHandle<EntityComponentBase> &index)
            {
                mHandle.relocate(index);
            }
        };

        template <typename T>
        bool operator<(uint32_t first, const EntityComponentOwningHandle<T> &second)
        {
            return first < second.mHandle.type();
        }

    }

}
}