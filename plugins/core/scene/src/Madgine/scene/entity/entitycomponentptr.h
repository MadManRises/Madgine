#pragma once

#include "Modules/keyvalue/virtualscope.h"


namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T>
        struct EntityComponentPtr : VirtualScopeBase {
            EntityComponentPtr(T *data, const EntityPtr &entity)
                : mData(data)
                , mEntity(entity)
            {
            }

            virtual TypedScopePtr customScopePtr() override {
                return 
            }

        protected:
            T *mData; //TODO ->  GenerationVector::iterator
            EntityPtr mEntity;
        };

    }
}
}