#pragma once

#include "Modules/serialize/serializabledataunit.h"
#include "Modules/keyvalue/objectptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        struct MADGINE_SCENE_EXPORT EntityComponentBase : ScopeBase, Serialize::SerializableDataUnit{ 
            EntityComponentBase(const ObjectPtr &initTable = {});
        };
    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentBase);