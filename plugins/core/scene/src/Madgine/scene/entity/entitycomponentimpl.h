#pragma once

#include "entityptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T>
        struct EntityComponentImplBase {
            EntityComponentImplBase(T *data, const EntityPtr &entity)
                : mData(data)
                , mEntity(entity)
            {
            }

        protected:
            T *mData;
            EntityPtr mEntity;
        };

        template <typename T>
        struct EntityComponentImpl : EntityComponentImplBase<T> {
            using EntityComponentImplBase<T>::EntityComponentImplBase;
        };

    }
}
}