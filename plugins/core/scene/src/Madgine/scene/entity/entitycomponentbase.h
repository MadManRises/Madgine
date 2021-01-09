#pragma once

#include "Modules/keyvalue/scopebase.h"
#include "Modules/serialize/serializableunit.h"
#include "Modules/keyvalue/objectptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        struct MADGINE_SCENE_EXPORT EntityComponentBase : ScopeBase, Serialize::SerializableUnitBase{        
            EntityComponentBase(const ObjectPtr &initTable = {});
        };
    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentBase);