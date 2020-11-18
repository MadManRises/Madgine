#pragma once

#include "Modules/generic/container/generationvector.h"
#include "entitycomponenthandle.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct EntityComponentListBase {
            virtual ~EntityComponentListBase() = default;

            virtual GenerationContainerBase *operator->() = 0;
            virtual EntityComponentBase *get(GenerationContainerIndex &index) = 0;
            virtual EntityHandle getEntity(GenerationContainerIndex &index, SceneManager *mgr) const = 0; 
            virtual EntityComponentOwningHandle<EntityComponentBase> emplace(const ObjectPtr &table, const EntityPtr &entity) = 0;
            virtual void erase(GenerationContainerIndex &index, GenerationVector<Entity> &vec) = 0;
            virtual bool empty() = 0;
            virtual void clear() = 0;
        };

    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentListBase);