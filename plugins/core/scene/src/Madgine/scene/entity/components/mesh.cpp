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
PROPERTY(Material, material, setMaterial)
METATABLE_END(Engine::Scene::Entity::Mesh)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Mesh)
ENCAPSULATED_FIELD(Mesh, getName, setName)
SERIALIZETABLE_END(Engine::Scene::Entity::Mesh)

namespace Engine {
namespace Scene {
    namespace Entity {
        Mesh::Mesh(const ObjectPtr &data)
            : NamedUniqueComponent(data)
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

        Render::GPUMeshData::Material *Mesh::material() const
        {
            return mMaterial;
        }

        void Mesh::setMaterial(Render::GPUMeshData::Material *material)
        {
            mMaterial = material;
        }

        void Mesh::setMaterialName(std::string_view name)
        {
            if (!mMesh)
                return;
            auto it = std::find_if(mMesh->mMaterials.begin(), mMesh->mMaterials.end(), [&](const Render::GPUMeshData::Material &mat) { return mat.mName == name; });
            if (it != mMesh->mMaterials.end())
                mMaterial = &*it;
        }

        std::string_view Mesh::getName() const
        {
            return mMesh.name();
        }

        void Mesh::setName(std::string_view name)
        {
            mMesh.load(name);
            mMaterial = mMesh && !mMesh->mMaterials.empty() ? &mMesh->mMaterials.front() : nullptr;
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
            mMesh = std::move(handle);
            mMaterial = mMesh ? &mMesh->mMaterials.front() : nullptr;
        }

        Render::GPUMeshLoader::ResourceType *Mesh::get() const
        {
            return mMesh.resource();
        }
    }
}
}
