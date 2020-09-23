#pragma once

#include "Modules/generic/container/generationvector.h"
#include "entitycomponenthandle.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct EntityComponentListBase {
            virtual ~EntityComponentListBase() = default;

            virtual GenerationVectorBase *operator->() = 0;
            virtual EntityComponentBase *get(GenerationVectorIndex &index) = 0;
            virtual EntityComponentOwningHandle<EntityComponentBase> emplace(const ObjectPtr &table) = 0;
            virtual void erase(GenerationVectorIndex &index) = 0;
            virtual bool empty() = 0;
            virtual void clear() = 0;
        };

    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentListBase);