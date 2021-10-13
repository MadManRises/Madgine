#pragma once

#include "Generic/container/container_api.h"
#include "Generic/container/multicontainer.h"
#include "Meta/keyvalue/typedscopeptr.h"
#include "Meta/serialize/serializableunitptr.h"
#include "entitycomponentcollector.h"
#include "entitycomponentlistbase.h"
#include "entityptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        DERIVE_FUNCTION(init, Entity *);
        DERIVE_FUNCTION(finalize, Entity *);
        DERIVE_FUNCTION(updateRender, Entity *);
        DERIVE_FUNCTION(relocateComponent, const EntityComponentHandle<EntityComponentBase> &, Entity *);

        MADGINE_SCENE_EXPORT void relocateEntityComponent(Entity *entity, EntityComponentHandle<EntityComponentBase> index);

        template <typename T>
        struct EntityComponentList : EntityComponentListComponent<EntityComponentList<T>> {

            using Config = typename T::Config;

            static_assert(sizeof(T) >= sizeof(IndexType<uint32_t>));
            static_assert(Config::sContiguous != Config::sPersistent);

            using Vector = std::conditional_t<Config::sPersistent, container_api<MultiContainer<std::deque, T, Entity *>>, container_api<MultiContainer<std::vector, T, Entity *>>>;

            Vector *operator->()
            {
                return &mData;
            }

            const Vector *operator->() const
            {
                return &mData;
            }

            T *get(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                return &mData.at(index.mIndex).template get<0>();
            }

            const T *get(const EntityComponentHandle<EntityComponentBase> &index) const override final
            {
                return &mData.at(index.mIndex).template get<0>();
            }

            TypedScopePtr getTyped(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                return &mData.at(index.mIndex).template get<0>();
            }

            Serialize::SerializableDataPtr getSerialized(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                return &mData.at(index.mIndex).template get<0>();
            }

            Serialize::SerializableDataConstPtr getSerialized(const EntityComponentHandle<EntityComponentBase> &index) const override final
            {
                return &mData.at(index.mIndex).template get<0>();
            }

            void init(const EntityComponentHandle<EntityComponentBase> &index, Entity *entity) override final
            {
                if constexpr (has_function_init_v<T>)
                    mData.at(index.mIndex).template get<0>().init(entity);
            }

            void finalize(const EntityComponentHandle<EntityComponentBase> &index, Entity *entity) override final
            {
                if constexpr (has_function_finalize_v<T>)
                    mData.at(index.mIndex).template get<0>().finalize(entity);
            }

            T *get(const EntityComponentHandle<T> &index)
            {
                return &mData.at(index.mIndex).template get<0>();
            }

            const T *get(const EntityComponentHandle<T> &index) const
            {
                return &mData.at(index.mIndex).template get<0>();
            }

            Entity *getEntity(const EntityComponentHandle<EntityComponentBase> &index) const override final
            {
                assert(index.mType == component_index<T>());
                if (index)
                    return getEntity(index.mIndex);
                else
                    return {};
            }

            Entity *getEntity(uint32_t index) const
            {
                return mData.at(index).template get<1>();
            }

            EntityComponentOwningHandle<EntityComponentBase> emplace(const ObjectPtr &table, Entity *entity) override final
            {
                if constexpr (Config::sPersistent) {
                    if (mFreeListHead) {
                        auto it = mData.begin() + mFreeListHead;
                        IndexType<uint32_t> &node = reinterpret_cast<IndexType<uint32_t> &>(it.template get<0>());
                        mFreeListHead = node;
                        new (&it.template get<0>()) T(table);
                        it.template get<1>() = entity;
                        return { { static_cast<uint32_t>(it - mData.begin()), static_cast<uint32_t>(component_index<T>()) } };
                    }
                }
                auto it = mData.emplace(mData.end(), std::piecewise_construct, std::forward_as_tuple(table), std::make_tuple(entity));
                return { { static_cast<uint32_t>(it - mData.begin()), static_cast<uint32_t>(component_index<T>()) } };
            }

            void erase(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                auto it = mData.begin() + index.mIndex;
                if constexpr (Config::sContiguous) {
                    auto last = --mData.end();
                    if (last != it) {
                        it.template get<0>() = std::move(last.template get<0>());
                        it.template get<1>() = std::exchange(last.template get<1>(), nullptr);
                        relocateEntityComponent(it.template get<1>(), index);
                    }
                    mData.erase(last);
                } else {
                    it.template get<1>() = nullptr;
                    it.template get<0>().~T();
                    IndexType<uint32_t> &node = reinterpret_cast<IndexType<uint32_t> &>(it.template get<0>());
                    node = mFreeListHead;
                    mFreeListHead = index.mIndex;
                }
            }

            bool empty() override final
            {
                return mData.empty();
            }

            void clear() override final
            {
                mData.clear();
            }

            size_t size() const override final
            {
                return mData.size();
            }

            void updateRender() override final
            {
                if constexpr (has_function_updateRender_v<T>) {
                    for (const std::tuple<T &, Entity *&> &t : mData) {
                        std::get<0>(t).updateRender(std::get<1>(t));
                    }
                }
            }

            typename std::vector<T>::iterator begin()
            {
                return mData.template get<0>().begin();
            }

            typename std::vector<T>::iterator end()
            {
                return mData.template get<0>().end();
            }

            Vector &data()
            {
                return mData;
            }

            T &front()
            {
                return mData.template get<0>().front();
            }

            T &operator[](size_t index)
            {
                return mData.template get<0>()[index];
            }

            Vector mData;
            IndexType<uint32_t> mFreeListHead;
        };

    }
}
}