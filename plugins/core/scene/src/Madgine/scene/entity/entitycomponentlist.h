#pragma once

#include "entitycomponentlistbase.h"
#include "Modules/generic/container/generationvector.h"
#include "entitycomponentcollector.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T>
        struct EntityComponentList : EntityComponentListComponent<EntityComponentList<T>> {

            GenerationVector<T>* operator->() override final {
                return &mData;
            }

            const GenerationVector<T> *operator->() const
            {
                return &mData;
            }

            T *get(GenerationVectorIndex &index) override final
            {
                return &mData.at(index);
            }

            const T *get(GenerationVectorIndex &index) const
            {
                return &mData.at(index);
            }

            EntityComponentOwningHandle<EntityComponentBase> emplace(const ObjectPtr &table) override final
            {
                return { mData.emplace(table).copyIndex() };
            }

            void erase(GenerationVectorIndex &index) override final
            {
                mData.erase(index);
            }

            typename GenerationVector<T>::iterator begin() {
                return mData.begin();
            }

            typename GenerationVector<T>::iterator end()
            {
                return mData.end();
            }

            GenerationVector<T> mData;
        };

    }
}
}