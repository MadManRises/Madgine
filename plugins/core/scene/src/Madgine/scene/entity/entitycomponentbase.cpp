#include "../../scenelib.h"
#include "entitycomponentbase.h"
#include "entity.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

namespace Engine {

namespace Scene {
    namespace Entity {
        EntityComponentBase::EntityComponentBase(const ObjectPtr &initTable)
        {
        }

        EntityComponentBase::~EntityComponentBase()
        {
        }

        void EntityComponentBase::init(const EntityPtr &entity)
        {
        }

        void EntityComponentBase::finalize(const EntityPtr &entity)
        {
        }

    }
}
}

METATABLE_BEGIN(Engine::Scene::Entity::EntityComponentBase)
METATABLE_END(Engine::Scene::Entity::EntityComponentBase)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::EntityComponentBase)
SERIALIZETABLE_END(Engine::Scene::Entity::EntityComponentBase)
