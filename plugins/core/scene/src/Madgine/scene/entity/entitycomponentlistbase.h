#pragma once

#include "Modules/generic/container/generationvector.h"
#include "entitycomponenthandle.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct EntityComponentListBase {
            virtual ~EntityComponentListBase() = default;

            virtual EntityComponentBase *get(const EntityComponentHandle<EntityComponentBase> &index) = 0;
            virtual const EntityComponentBase *get(const EntityComponentHandle<EntityComponentBase> &index) const = 0;
            virtual TypedScopePtr getTyped(const EntityComponentHandle<EntityComponentBase> &index) = 0;
            virtual void init(const EntityComponentHandle<EntityComponentBase> &index, Entity *entity) = 0;
            virtual void finalize(const EntityComponentHandle<EntityComponentBase> &index, Entity *entity) = 0;
            virtual Entity *getEntity(const EntityComponentHandle<EntityComponentBase> &index) const = 0; 
            virtual EntityComponentOwningHandle<EntityComponentBase> emplace(const ObjectPtr &table, Entity *entity) = 0;
            virtual void erase(const EntityComponentHandle<EntityComponentBase> &index) = 0;
            virtual bool empty() = 0;
            virtual void clear() = 0;

            virtual void readState(const EntityComponentHandle<EntityComponentBase> &index, Serialize::SerializeInStream &in, const char *name = nullptr, CallerHierarchyBasePtr hierarchy = {}) = 0;
            virtual void writeState(const EntityComponentHandle<EntityComponentBase> &index, Serialize::SerializeOutStream &out, const char *name = nullptr, CallerHierarchyBasePtr hierarchy = {}) const = 0;
        };

    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentListBase);