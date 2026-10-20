#include "../../../scenelib.h"

#include "mesh.h"

#include "Meta/math/boundingbox.h"

#include "Meta/reflect/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "../entity.h"

NAMED_UNIQUECOMPONENT(Mesh, Engine::Scene::Entity::Mesh);

METATABLE_BEGIN_BASE(Engine::Scene::Entity::Mesh, Engine::Scene::Entity::EntityComponentBase)
    STORAGE_BEGIN(Engine::Scene::Entity::Mesh, Engine::Scene::Entity::Mesh)
    CONSTRUCTOR()
    STORAGE_END(Engine::Scene::Entity::Mesh)
    MEMBER(mMesh)
    PROPERTY(Visible, visible, setVisible)
    PROPERTY(InheritVisibility, inheritsVisibility, setInheritVisibility)
    MEMBER(mMaterial)
METATABLE_END(Engine::Scene::Entity::Mesh)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Mesh)
    FIELD(mMesh)
    FIELD(mMaterial)
SERIALIZETABLE_END(Engine::Scene::Entity::Mesh)

namespace Engine {
namespace Scene {
    namespace Entity {

        const Render::GPUMeshData *Mesh::data() const
        {
            return mMesh;
        }

        Math::AABB Mesh::aabb() const
        {
            return mMesh->mAABB;
        }

        void Mesh::setVisible(bool visible)
        {
            mVisibility = visible ? Visible : Invisible;
        }

        void Mesh::setInheritVisibility(bool inherit)
        {
            mVisibility = inherit ? Inherit : Visible;
        }

        bool Mesh::visible(Contextual<Entity &> entity) const
        {
            if (mVisibility == Inherit) {
                EntityPtr parent = entity->parent();

                bool visible = true;

                while (Execution::access_binding(parent, [&](Entity &e) {
                    Mesh *mesh = e.getComponent<Mesh>();
                    if (mesh) {
                        visible = mesh->visible(e);
                        return false;
                    }
                    parent = e.parent();
                    return true;
                }))
                    ;
                return visible;
            } else {
                return mVisibility == Visible;
            }
        }

        bool Mesh::inheritsVisibility() const
        {
            return mVisibility == Inherit;
        }

    }
}
}
