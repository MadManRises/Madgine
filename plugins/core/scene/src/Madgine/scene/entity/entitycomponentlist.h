#pragma once

#include "Generic/container/container_api.h"
#include "Generic/container/multicontainer.h"
#include "Meta/keyvalue/typedscopeptr.h"
#include "Meta/serialize/serializableunitptr.h"
#include "entitycomponentcollector.h"
#include "entitycomponentlistbase.h"
#include "Generic/container/freelistcontainer.h"

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

            struct EntityPtrDummy {
                Entity *mPtr = nullptr;
                EntityPtrDummy(Entity *ptr)
                    : mPtr(ptr)
                {
                }
                ~EntityPtrDummy() {
                    mPtr = nullptr;
                }
                operator Entity* () const {
                    return mPtr;
                }

            };

            static_assert(sizeof(T) >= sizeof(uintptr_t));
            static_assert(Config::sContiguous != Config::sPersistent);

            static bool isFree(std::tuple<T &, EntityPtrDummy &> data)
            {
                return !std::get<1>(data);
            }

            static uintptr_t *getLocation(std::tuple<T &, EntityPtrDummy &> data)
            {
                return &reinterpret_cast<uintptr_t &>(std::get<0>(data));
            }

            using Vector = std::conditional_t<Config::sPersistent, container_api<FreeListContainer<MultiContainer<std::deque, T, EntityPtrDummy>, EntityComponentList<T>>>, container_api<MultiContainer<std::vector, T, Entity *>>>;

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
                typename Vector::iterator it = mData.emplace(mData.end(), std::piecewise_construct, std::forward_as_tuple(table), std::make_tuple(entity));
                uint32_t index = container_traits<Vector>::toHandle(mData, it);
                return { { index, static_cast<uint32_t>(component_index<T>()) } };
            }

            void erase(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                auto it = container_traits<Vector>::toIterator(mData, index.mIndex);
                if constexpr (Config::sContiguous) {
                    auto it = mData.begin() + index.mIndex;
                    auto last = --mData.end();
                    if (last != it) {
                        it.template get<0>() = std::move(last.template get<0>());
                        it.template get<1>() = std::exchange(last.template get<1>(), nullptr);
                        relocateEntityComponent(it.template get<1>(), index);
                    }
                    mData.erase(last);
                } else {
                    mData.erase(it);
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
        };

    }
}
}