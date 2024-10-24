#include "../../../scenelib.h"

#include "material.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

ENTITYCOMPONENT_IMPL(Material, Engine::Scene::Entity::Material);

METATABLE_BEGIN(Engine::Scene::Entity::Material)
MEMBER(mMaterial)
METATABLE_END(Engine::Scene::Entity::Material)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Material)

SERIALIZETABLE_END(Engine::Scene::Entity::Material)

namespace Engine {
namespace Scene {
    namespace Entity {
        Material::Material(const ObjectPtr &data)
            : NamedComponent(data)
        {
        }

        const Render::GPUMeshData::Material *Material::get() const
        {
            return &mMaterial;
        }

    }
}
}
