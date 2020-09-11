#include "../bulletlib.h"

#include "collisionshapemanager.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "meshloader.h"

UNIQUECOMPONENT(Engine::Physics::CollisionShapeManager);

METATABLE_BEGIN(Engine::Physics::CollisionShapeManager::ResourceType)
METATABLE_END(Engine::Physics::CollisionShapeManager::ResourceType)

METATABLE_BEGIN(Engine::Physics::CollisionShapeManager)
MEMBER(mResources)
METATABLE_END(Engine::Physics::CollisionShapeManager)

namespace Engine {
namespace Physics {

    struct MeshShape : CollisionShape {
        static btTriangleIndexVertexArray *setup(btTriangleIndexVertexArray &array, const btIndexedMesh &meshData)
        {
            array.addIndexedMesh(meshData, PHY_SHORT);
            return &array;
        }

        MeshShape(const btIndexedMesh &meshData)
            : mShape(setup(mVertexArray, meshData), true)
        {
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        btTriangleIndexVertexArray mVertexArray;
        btBvhTriangleMeshShape mShape;
    };

    CollisionShapeManager::CollisionShapeManager()
        : ResourceLoader({ ".fbx", ".dae" })
    {
    }

    bool CollisionShapeManager::loadImpl(std::unique_ptr<CollisionShape> &shape, ResourceType *res)
    {
        Render::MeshLoader::HandleType mesh;
        mesh.load(res->name());
        if (!mesh)
            return false;

        assert(!mesh->mIndices.empty());

        btIndexedMesh meshData;
        meshData.m_numVertices = mesh->mVertices.mSize / mesh->mVertexSize;
        meshData.m_numTriangles = mesh->mIndices.size() / mesh->mGroupSize;
        meshData.m_triangleIndexBase = reinterpret_cast<unsigned char *>(mesh->mIndices.data());
        meshData.m_triangleIndexStride = 3 * sizeof(unsigned short);
        meshData.m_vertexBase = reinterpret_cast<const unsigned char *>(mesh->mVertices.mData);
        meshData.m_vertexStride = mesh->mVertexSize;

        shape = std::make_unique<MeshShape>(meshData);
        return true;
    }

    void CollisionShapeManager::unloadImpl(std::unique_ptr<CollisionShape> &shape, ResourceType *res)
    {
    }

}
}