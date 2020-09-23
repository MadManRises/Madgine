#include "../bulletlib.h"

#include "collisionshapemanager.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "meshloader.h"

#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btBoxShape.h"

UNIQUECOMPONENT(Engine::Physics::CollisionShapeManager);

METATABLE_BEGIN(Engine::Physics::CollisionShapeManager::ResourceType)
METATABLE_END(Engine::Physics::CollisionShapeManager::ResourceType)

METATABLE_BEGIN(Engine::Physics::CollisionShapeManager)
MEMBER(mResources)
METATABLE_END(Engine::Physics::CollisionShapeManager)

namespace Engine {
namespace Physics {

    struct MeshShape : CollisionShape {

        MeshShape(const ByteBuffer &vertexData, size_t vertexSize)
            : mShape(static_cast<const float *>(vertexData.mData), vertexData.mSize / vertexSize, vertexSize)
        {
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }
        
        btConvexHullShape mShape;
    };

    struct BoxShape : CollisionShape {

        BoxShape()
            : mShape({ 0.5f, 0.5f, 0.5f })
        {
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        btBoxShape mShape;
    };

    CollisionShapeManager::CollisionShapeManager()
        : ResourceLoader({ ".fbx", ".dae" })
    {
        getOrCreateManual(
            "Cube", {}, [](CollisionShapeManager *mgr, std::unique_ptr<CollisionShape> &data, ResourceType *res) {
                data = std::make_unique<BoxShape>();
                return true;
            },
            {}, this);
    }

    bool CollisionShapeManager::loadImpl(std::unique_ptr<CollisionShape> &shape, ResourceType *res)
    {
        Render::MeshLoader::HandleType mesh;
        mesh.load(res->name());
        if (!mesh)
            return false;

        shape = std::make_unique<MeshShape>(mesh->mVertices, mesh->mVertexSize);
        return true;
    }

    void CollisionShapeManager::unloadImpl(std::unique_ptr<CollisionShape> &shape, ResourceType *res)
    {
    }

}
}