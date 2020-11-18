#pragma once

#include "Modules/generic/container/generationvector.h"
#include "Modules/generic/container/multivector.h"
#include "entitycomponentcollector.h"
#include "entitycomponentlistbase.h"
#include "entityhandle.h"
#include "entityptr.h"
#include "Modules/generic/container/container_api.h"
#include "../scenemanager.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T>
        struct EntityComponentList : EntityComponentListComponent<EntityComponentList<T>> {

            using Vector = container_api<GenerationContainer<MultiVector<T, EntityHandle>>>;

            Vector *operator->() override final
            {
                return &mData;
            }

            const Vector *operator->() const
            {
                return &mData;
            }

            T *get(GenerationContainerIndex &index) override final
            {
                return &mData.at(index).template get<0>();
            }

            const T *get(GenerationContainerIndex &index) const
            {
                return &mData.at(index).template get<0>();
            }

            EntityHandle getEntity(GenerationContainerIndex& index, SceneManager *mgr) const override final {
                mData.update(index);
                if (index)
                    return mgr->copyEntityHandle(mData.at(index).template get<1>());
                else
                    return {};
            }

            EntityComponentOwningHandle<EntityComponentBase> emplace(const ObjectPtr &table, const EntityPtr &entity) override final
            {
                return {
                    mData.emplace(std::piecewise_construct, std::forward_as_tuple(table), std::make_tuple(entity)).copyIndex()
                };
            }

            void erase(GenerationContainerIndex &index, GenerationVector<Entity> &vec) override final
            {
                vec.reset(mData.at(index).template get<1>().mIndex);
                mData.erase(index);
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

            T& front() {
                return mData.template get<0>().front();
            }

            Vector mData;
        };

    }
}
}