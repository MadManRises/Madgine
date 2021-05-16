#include "../../../scenelib.h"

#include "mesh.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/math/boundingbox.h"

#include "Meta/keyvalue/valuetype.h"

#include "meshdata.h"

#include "../entitycomponentptr.h"


ENTITYCOMPONENT_IMPL(Mesh, Engine::Scene::Entity::Mesh);

METATABLE_BEGIN(Engine::Scene::Entity::Mesh)
PROPERTY(Mesh, get, set)
PROPERTY(Visible, isVisible, setVisible)
METATABLE_END(Engine::Scene::Entity::Mesh)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Mesh)
ENCAPSULATED_FIELD(Mesh, getName, setName)
SERIALIZETABLE_END(Engine::Scene::Entity::Mesh)


namespace Engine {
namespace Scene {
    namespace Entity {
        Mesh::Mesh(const ObjectPtr &data)
            : EntityComponent(data)
        {
            Engine::ValueType v;
            if (data.getValue(v, "mesh") && v.is<std::string>()) {
                setName(v.as<std::string>());
            }
        }
        Render::GPUMeshData *Mesh::data() const
        {
            return mMesh;
        }

        std::string_view Mesh::getName() const
        {
            return mMesh.name();
        }

        void Mesh::setName(const std::string_view &name)
        {
            mMesh.load(name);
        }

        void Mesh::setVisible(bool vis)
        {
            mIsVisible = vis;
        }

        bool Mesh::isVisible() const
        {
            return mIsVisible;
        }

        AABB Mesh::aabb() const
        {
            return mMesh->mAABB;
        }

        void Mesh::set(Render::GPUMeshLoader::HandleType handle)
        {
            mMesh = handle;
        }

        Render::GPUMeshLoader::ResourceType *Mesh::get() const
        {
            return mMesh.resource();
        }
    }
}
}


