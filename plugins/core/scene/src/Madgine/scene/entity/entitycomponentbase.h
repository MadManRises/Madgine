#pragma once

#include "Meta/serialize/serializabledataunit.h"
#include "Meta/keyvalue/objectptr.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct PersistentComponentConfig {
            static constexpr bool sPersistent = true;
            static constexpr bool sContiguous = false;
        };

        struct ContiguousComponentConfig {
            static constexpr bool sPersistent = false;
            static constexpr bool sContiguous = true;
        };

        struct MADGINE_SCENE_EXPORT EntityComponentBase : Serialize::SerializableDataUnit{ 

            using Config = ContiguousComponentConfig;
            
            EntityComponentBase(const std::optional<ObjectPtr> &initTable = {});
        };
    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentBase);