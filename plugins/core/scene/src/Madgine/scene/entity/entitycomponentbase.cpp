#include "../../scenelib.h"
#include "entitycomponentbase.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Scene::Entity::EntityComponentBase)
METATABLE_END(Engine::Scene::Entity::EntityComponentBase)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::EntityComponentBase)
SERIALIZETABLE_END(Engine::Scene::Entity::EntityComponentBase)

namespace Engine {
namespace Scene {
    namespace Entity {

        EntityComponentBase::EntityComponentBase(const std::optional<ObjectPtr> &initTable)
        {
        }

    }
}
}
