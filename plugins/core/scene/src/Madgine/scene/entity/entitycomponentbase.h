#pragma once

#include "Meta/serialize/serializabledataunit.h"
#include "Meta/keyvalue/objectptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        struct MADGINE_SCENE_EXPORT EntityComponentBase : Serialize::SerializableDataUnit{ 
            EntityComponentBase(const std::optional<ObjectPtr> &initTable = {});
        };
    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentBase);