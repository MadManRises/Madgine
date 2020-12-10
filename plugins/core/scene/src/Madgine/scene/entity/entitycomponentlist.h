#pragma once

#include "Modules/generic/container/container_api.h"
#include "Modules/generic/container/generationvector.h"
#include "Modules/generic/container/multivector.h"
#include "Modules/keyvalue/typedscopeptr.h"
#include "entitycomponentcollector.h"
#include "entitycomponentlistbase.h"
#include "entityhandle.h"
#include "entityptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        MODULES_EXPORT EntityHandle copyEntityHandle(SceneManager *mgr, const EntityHandle &entity);

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

            TypedScopePtr getTyped(const EntityComponentHandle<EntityComponentBase> &index) override final
            {
                return &mData.at(index.mIndex).template get<0>();
            }

            void init(const EntityComponentHandle<EntityComponentBase> &index, Entity *entity) override final
            {
            }

            void finalize(const EntityComponentHandle<EntityComponentBase> &index, Entity *entity) override final
            {
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
                return
                {
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