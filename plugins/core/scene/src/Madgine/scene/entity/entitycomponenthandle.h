#pragma once

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Scene {

    namespace Entity {

        template <typename T>
        struct EntityComponentHandle;

        void entityComponentHelperWrite(Serialize::SerializeOutStream &out, const EntityComponentHandle<EntityComponentBase> &index, const char *name, SceneManager *mgr);
        void entityComponentHelperRead(Serialize::SerializeInStream &in, EntityComponentHandle<EntityComponentBase> &index, const char *name, SceneManager *mgr);

        template <>
        struct EntityComponentHandle<EntityComponentBase> {
            uint32_t mIndex = std::numeric_limits<uint32_t>::max();
            uint32_t mType = std::numeric_limits<uint32_t>::max();

            void readState(Serialize::SerializeInStream &in, const char *name, SceneManager *mgr)
            {
                entityComponentHelperRead(in, *this, name, mgr);
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
                return mIndex != std::numeric_limits<uint32_t>::max();
            }

            uint32_t type() const
            {
                return mType;
            }
        };

        template <>
        struct EntityComponentHandle<const EntityComponentBase> {
            uint32_t mIndex = std::numeric_limits<uint32_t>::max();
            uint32_t mType = std::numeric_limits<uint32_t>::max();

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
                return mIndex != std::numeric_limits<uint32_t>::max();
            }

            uint32_t type() const
            {
                return mType;
            }
        };

        template <typename T>
        struct EntityComponentHandle {
            uint32_t mIndex = std::numeric_limits<uint32_t>::max();

            EntityComponentHandle() = default;

            EntityComponentHandle(const EntityComponentHandle<EntityComponentBase> &other)
                : mIndex(other.mIndex)
            {
                assert(!other || component_index<T>() == other.mType);
            }

            void readState(Serialize::SerializeInStream &in, const char *name, SceneManager *mgr)
            {
                EntityComponentHandle<EntityComponentBase> handle { std::numeric_limits<uint32_t>::max(), type() };
                entityComponentHelperRead(in, handle, name, mgr);
                mIndex = handle.mIndex;
            }

            void writeState(Serialize::SerializeOutStream &out, const char *name, SceneManager *mgr) const
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
                return mIndex != std::numeric_limits<uint32_t>::max();
            }

            uint32_t type() const
            {
                return component_index<T>();
            }

            operator EntityComponentHandle<EntityComponentBase>() const
            {
                return { mIndex, type() };
            }
        };

        template <typename T>
        struct EntityComponentOwningHandle {
            EntityComponentHandle<T> mHandle;

            operator EntityComponentHandle<T>() const
            {
                return mHandle;
            }

            bool operator<(const EntityComponentOwningHandle<T> &other) const
            {
                return mHandle.type() < other.mHandle.type();
            }

            bool operator<(uint32_t type) const
            {
                return mHandle.type() < type;
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