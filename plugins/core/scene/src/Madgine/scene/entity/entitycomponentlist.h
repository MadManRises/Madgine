#pragma once

#include "Generic/container/container_api.h"
#include "Generic/container/multivector.h"
#include "Meta/keyvalue/typedscopeptr.h"
#include "entitycomponentcollector.h"
#include "entitycomponentlistbase.h"
#include "entityptr.h"
#include "Meta/serialize/serializableunitptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        DERIVE_FUNCTION(init, Entity *);
        DERIVE_FUNCTION(finalize, Entity *);

        template <typename T>
        struct EntityComponentList : EntityComponentListComponent<EntityComponentList<T>> {

            using Vector = container_api<MultiVector<T, Entity *>>;

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
                    return mData.at(index.mIndex).template get<1>();
                else
                    return {};
            }

            EntityComponentOwningHandle<EntityComponentBase> emplace(const ObjectPtr &table, Entity *entity) override final
            {
                return {
                    static_cast<uint32_t>(&mData.emplace_back(std::piecewise_construct, std::forward_as_tuple(table), std::make_tuple(entity)) - mData.data()), static_cast<uint32_t>(component_index<T>())
                };
            }

            void erase(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                //mData.erase(mData.begin() + index.mIndex);
            }

            bool empty() override final
            {
                return mData.empty();
            }

            void clear() override final
            {
                mData.clear();
            }

            typename std::vector<T>::iterator begin()
            {
                return mData.template get<0>().begin();
            }

            typename std::vector<T>::iterator end()
            {
                return mData.template get<0>().end();
            }

            T &front()
            {
                return mData.template get<0>().front();
            }

            Vector mData;
        };

    }
}
}