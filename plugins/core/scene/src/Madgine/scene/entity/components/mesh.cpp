#include "../../../scenelib.h"

#include "mesh.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/math/boundingbox.h"

#include "meshdata.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        Mesh::Mesh(Scene::Entity::Entity &e, const ObjectPtr &data)
            : EntityComponent(e, data)
        {
            if (const Engine::ValueType &v = data["mesh"]; v.is<std::string>()) {
                setName(v.as<std::string>());
            }
        }
        Render::MeshData *Mesh::data() const
        {
            return mMesh;
        }

        std::string Mesh::getName() const
        {
            return mMesh.name();
        }

        void Mesh::setName(const std::string &name)
        {
            mMesh.load(name);
        }

        void Mesh::setVisible(bool vis)
        {
        }

        bool Mesh::isVisible() const
        {
            return true;
        }

        AABB Mesh::aabb() const
        {
            return mMesh->mAABB;
        }

        void Mesh::setManual(Render::MeshLoader::HandleType handle)
        {
            mMesh = handle;
        }

        void Mesh::set(Render::MeshLoader::ResourceType *res)
        {
            mMesh = res;
        }

        Render::MeshLoader::ResourceType *Mesh::get() const
        {
            return mMesh.resource();
        }
    }
}
}

ENTITYCOMPONENT_IMPL(Mesh, Engine::Scene::Entity::Mesh);

METATABLE_BEGIN(Engine::Scene::Entity::Mesh)
PROPERTY(Mesh, get, set)
METATABLE_END(Engine::Scene::Entity::Mesh)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Mesh)
ENCAPSULATED_FIELD(Mesh, getName, setName)
SERIALIZETABLE_END(Engine::Scene::Entity::Mesh)

RegisterType(Engine::Scene::Entity::Mesh);
