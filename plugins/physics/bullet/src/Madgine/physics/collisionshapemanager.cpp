#include "../bulletlib.h"

#include "collisionshapemanager.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "meshloader.h"

#include "Modules/math/quaternion.h"

#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btBoxShape.h"
#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btCompoundShape.h"
#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btConvexHullShape.h"

UNIQUECOMPONENT(Engine::Physics::CollisionShapeManager);

METATABLE_BEGIN(Engine::Physics::CollisionShapeManager::ResourceType)
METATABLE_END(Engine::Physics::CollisionShapeManager::ResourceType)

METATABLE_BEGIN(Engine::Physics::CollisionShapeManager)
MEMBER(mResources)
METATABLE_END(Engine::Physics::CollisionShapeManager)

METATABLE_BEGIN(Engine::Physics::CollisionShapeInstance)
METATABLE_END(Engine::Physics::CollisionShapeInstance)

namespace Engine {
namespace Physics {

    struct MeshShape : CollisionShape, VirtualScope<MeshShape, CollisionShapeInstance> {

        MeshShape(const ByteBuffer &vertexData, size_t vertexSize)
            : mShape(static_cast<const float *>(vertexData.mData), vertexData.mSize / vertexSize, vertexSize)
        {
        }

        virtual CollisionShapeInstance *create() override
        {
            return this;
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual bool isInstance() override
        {
            return false;
        }

        btConvexHullShape mShape;
    };

    struct BoxShapeInstance : VirtualScope<BoxShapeInstance, CollisionShapeInstance> {
        BoxShapeInstance()
            : mShape({ 0.5f, 0.5f, 0.5f })
        {
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual bool isInstance() override
        {
            return true;
        }

        Vector3 getHalfExtends() const
        {
            return mShape.getHalfExtentsWithoutMargin().m_floats;
        }

        void setHalfExtends(const Vector3 &halfExtends)
        {
            mShape.setImplicitShapeDimensions({ halfExtends.x, halfExtends.y, halfExtends.z });
        }

        btBoxShape mShape;
    };

    struct BoxShape : CollisionShape {

        virtual BoxShapeInstance *create() override
        {
            return new BoxShapeInstance;
        }
    };

    struct CompoundShapeInstance : VirtualScope<CompoundShapeInstance, CollisionShapeInstance> {

        void addShape(CollisionShapeManager::ResourceType *res)
        {
            CollisionShapeManager::InstanceHandle &instance = mChildren.emplace_back(res);
            mShape.addChildShape(btTransform { btQuaternion { 0, 0, 0 } }, instance->get());
        }

        struct CompoundShapeElement : ScopeBase {
            Vector3 mPos;
            Quaternion mOrientation;
            CollisionShapeInstance *mShape;
        };

        std::vector<CompoundShapeElement> shapes()
        {
            std::vector<CompoundShapeElement> result;
            result.resize(mChildren.size());
            for (size_t i = 0; i < mChildren.size(); ++i) {
                CompoundShapeElement &el = result[i];
                const btTransform &t = mShape.getChildTransform(i);
                el.mPos = t.getOrigin().m_floats;
                el.mOrientation = Quaternion { t.getRotation() };
                el.mShape = mChildren[i];
            }
            return result;
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual bool isInstance() override
        {
            return true;
        }

        std::vector<CollisionShapeManager::InstanceHandle> mChildren;
        btCompoundShape mShape;
    };

    struct CompoundShape : CollisionShape {
        virtual CompoundShapeInstance *create() override
        {
            return new CompoundShapeInstance;
        }
    };

    CollisionShapeManager::InstanceHandle::InstanceHandle(typename CollisionShapeManager::HandleType shape)
        : mInstance(shape->get()->create())
    {
        mInstance->mShape = shape;
    }

    CollisionShapeManager::InstanceHandle::InstanceHandle(typename CollisionShapeManager::ResourceType *res)
        : InstanceHandle(res->loadData())
    {
    }

    CollisionShapeManager::InstanceHandle::InstanceHandle(InstanceHandle &&other)
        : mInstance(std::exchange(other.mInstance, nullptr))
    {
    }

    CollisionShapeManager::InstanceHandle::~InstanceHandle()
    {
        if (mInstance && mInstance->isInstance())
            delete mInstance;
    }

    CollisionShapeManager::InstanceHandle &CollisionShapeManager::InstanceHandle::operator=(InstanceHandle &&other)
    {
        std::swap(mInstance, other.mInstance);
        return *this;
    }

    void CollisionShapeManager::InstanceHandle::load(const std::string_view &name, CollisionShapeManager *loader)
    {
        *this = CollisionShapeManager::load(name, loader);
    }

    CollisionShapeManager::ResourceType *CollisionShapeManager::InstanceHandle::resource() const
    {
        return mInstance ? mInstance->mShape.resource() : nullptr;
    }

    CollisionShapeInstance *CollisionShapeManager::InstanceHandle::operator->() const
    {
        return mInstance;
    }

    CollisionShapeManager::InstanceHandle::operator CollisionShapeInstance *() const
    {
        return mInstance;
    }

    CollisionShapeManager::CollisionShapeManager()
        : ResourceLoader({ ".fbx", ".dae" })
    {
        getOrCreateManual(
            "Cube", {}, [](CollisionShapeManager *mgr, std::unique_ptr<CollisionShape> &data, ResourceType *res) {
                data = std::make_unique<BoxShape>();
                return true;
            },
            {}, this);

        getOrCreateManual(
            "Compound", {}, [](CollisionShapeManager *mgr, std::unique_ptr<CollisionShape> &data, ResourceType *res) {
                data = std::make_unique<CompoundShape>();
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

METATABLE_BEGIN_BASE(Engine::Physics::MeshShape, Engine::Physics::CollisionShapeInstance)
METATABLE_END(Engine::Physics::MeshShape)

METATABLE_BEGIN_BASE(Engine::Physics::BoxShapeInstance, Engine::Physics::CollisionShapeInstance)
PROPERTY(halfExtends, getHalfExtends, setHalfExtends)
METATABLE_END(Engine::Physics::BoxShapeInstance)

METATABLE_BEGIN_BASE(Engine::Physics::CompoundShapeInstance, Engine::Physics::CollisionShapeInstance)
READONLY_PROPERTY(Shapes, shapes)
FUNCTION(addShape, Shape)
METATABLE_END(Engine::Physics::CompoundShapeInstance)

METATABLE_BEGIN(Engine::Physics::CompoundShapeInstance::CompoundShapeElement)
MEMBER(mPos)
MEMBER(mOrientation)
MEMBER(mShape)
METATABLE_END(Engine::Physics::CompoundShapeInstance::CompoundShapeElement)