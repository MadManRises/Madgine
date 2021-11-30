#pragma once

#include "Meta/serialize/serializabledataunit.h"
#include "Meta/keyvalue/objectptr.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "entitycomponentcontainer.h"

#include "Generic/container/compactcontainer.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT EntityComponentBase : Serialize::SerializableDataUnit{ 
            using Container = CompactContainer<EntityComponentContainerImpl<std::vector>, EntityComponentRelocateFunctor>;
            
            EntityComponentBase(const std::optional<ObjectPtr> &initTable = {});
        };
    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentBase);