#pragma once

#include "Generic/container/container_api.h"
#include "Generic/container/freelistcontainer.h"
#include "Meta/keyvalue/typedscopeptr.h"
#include "Meta/serialize/hierarchy/serializableunitptr.h"
#include "entitycomponentcollector.h"
#include "entitycomponentlistbase.h"
#include "entitycomponentcontainer.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        DERIVE_FUNCTION(init, Entity *)
        DERIVE_FUNCTION(finalize, Entity *)
        DERIVE_FUNCTION(updateRender, Entity *)
        DERIVE_FUNCTION(relocateComponent, const EntityComponentHandle<EntityComponentBase> &, Entity *)

        
        template <typename T>
        struct EntityComponentList : EntityComponentListComponent<EntityComponentList<T>> {

            using Vector = container_api<typename replace<typename T::Container>::template type<T>>;

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
                return &(mData.at(index.mIndex).template get<0>());
            }

            const T *get(const EntityComponentHandle<EntityComponentBase> &index) const override final
            {
                return &(mData.at(index.mIndex).template get<0>());
            }

            TypedScopePtr getTyped(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                return &(mData.at(index.mIndex).template get<0>());
            }

            Serialize::SerializableDataPtr getSerialized(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                return &(mData.at(index.mIndex).template get<0>());
            }

            Serialize::SerializableDataConstPtr getSerialized(const EntityComponentHandle<EntityComponentBase> &index) const override final
            {
                return &(mData.at(index.mIndex).template get<0>());
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
                return &(mData.at(index.mIndex).template get<0>());
            }

            const T *get(const EntityComponentHandle<T> &index) const
            {
                return &(mData.at(index.mIndex).template get<0>());
            }

            Entity *getEntity(const EntityComponentHandle<EntityComponentBase> &index) const override final
            {
                assert(index.mType == UniqueComponent::component_index<T>());
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
                return { { index, static_cast<uint32_t>(UniqueComponent::component_index<T>()) } };
            }

            void erase(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                auto it = container_traits<Vector>::toIterator(mData, index.mIndex);
                mData.erase(it);
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
                    for (const std::tuple<T &, NulledPtr<Entity> &> &t : mData) {
                        std::get<0>(t).updateRender(std::get<1>(t));
                    }
                }
            }

            auto begin()
            {
                return mData.begin();
            }

            auto end()
            {
                return mData.end();
            }

            auto &data()
            {
                return mData;
            }

            T &front()
            {
                return mData.front().template get<0>();
            }

            T &operator[](size_t index)
            {
                return mData[index].template get<0>();
            }

            Vector mData;
        };

    }
}
}