#include "../bulletlib.h"

#include "collisionshapemanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "meshloader.h"

#include "Meta/math/quaternion.h"

#include "Meta/serialize/streams/operations.h"

#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btBoxShape.h"
#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btCompoundShape.h"
#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btSphereShape.h"
#include "bullet3-2.89/src/BulletCollision/CollisionShapes/btStaticPlaneShape.h"

UNIQUECOMPONENT(Engine::Physics::CollisionShapeManager);

METATABLE_BEGIN(Engine::Physics::CollisionShapeManager::ResourceType)
METATABLE_END(Engine::Physics::CollisionShapeManager::ResourceType)

METATABLE_BEGIN(Engine::Physics::CollisionShapeManager)
MEMBER(mResources)
METATABLE_END(Engine::Physics::CollisionShapeManager)

METATABLE_BEGIN(Engine::Physics::CollisionShapeInstance)
METATABLE_END(Engine::Physics::CollisionShapeInstance)

SERIALIZETABLE_BEGIN(Engine::Physics::CollisionShapeInstance)
SERIALIZETABLE_END(Engine::Physics::CollisionShapeInstance)

namespace Engine {
namespace Physics {

    struct MeshShape : CollisionShape, Serialize::VirtualUnit<MeshShape, VirtualScope<MeshShape, CollisionShapeInstance>> {

        ~MeshShape()
        {
        }

        void setMesh(const ByteBuffer &vertexData, size_t vertexSize)
        {
            mShape = btConvexHullShape(static_cast<const float *>(vertexData.mData), vertexData.mSize / vertexSize, vertexSize);
        }

        virtual CollisionShapeInstancePtr create(typename CollisionShapeManager::HandleType shape) override
        {
            ++mRefCount;
            mHandle = std::move(shape);
            return CollisionShapeInstancePtr { this };
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual CollisionShapeInstancePtr clone() override
        {
            return CollisionShapeInstancePtr { this };
        }

        virtual void destroy() override
        {
            assert(mRefCount > 0);
            if (--mRefCount == 0) {
                mHandle.reset();
            }
        }

        std::atomic<uint32_t> mRefCount = 0;
        btConvexHullShape mShape;
    };

    struct BoxShapeInstance : Serialize::VirtualUnit<BoxShapeInstance, VirtualScope<BoxShapeInstance, CollisionShapeInstance>> {
        BoxShapeInstance(typename CollisionShapeManager::HandleType shape)
            : VirtualUnit(std::move(shape))
            , mShape({ 0.5f, 0.5f, 0.5f })
        {
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual void destroy() override
        {
            delete this;
        }

        virtual CollisionShapeInstancePtr clone() override
        {
            return CollisionShapeInstancePtr { new BoxShapeInstance(*this) };
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

        virtual CollisionShapeInstancePtr create(typename CollisionShapeManager::HandleType shape) override
        {
            return CollisionShapeInstancePtr { new BoxShapeInstance(std::move(shape)) };
        }
    };

    struct PlaneShapeInstance : Serialize::VirtualUnit<PlaneShapeInstance, VirtualScope<PlaneShapeInstance, CollisionShapeInstance>> {
        PlaneShapeInstance(typename CollisionShapeManager::HandleType shape)
            : VirtualUnit(std::move(shape))
            , mShape({ 0, 1, 0 }, 0.0f)
        {
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual void destroy() override
        {
            delete this;
        }

        virtual std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter> clone() override
        {
            return std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter> { new PlaneShapeInstance(*this) };
        }

        btStaticPlaneShape mShape;
    };

    struct PlaneShape : CollisionShape {

        virtual CollisionShapeInstancePtr create(typename CollisionShapeManager::HandleType shape) override
        {
            return CollisionShapeInstancePtr { new PlaneShapeInstance(std::move(shape)) };
        }
    };

    struct CapsuleShapeInstance : Serialize::VirtualUnit<CapsuleShapeInstance, VirtualScope<CapsuleShapeInstance, CollisionShapeInstance>> {
        CapsuleShapeInstance(typename CollisionShapeManager::HandleType shape)
            : VirtualUnit(std::move(shape))
            , mShape(0.5f, 2.0f)
        {
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual void destroy() override
        {
            delete this;
        }

        virtual std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter> clone() override
        {
            return std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter> { new CapsuleShapeInstance(*this) };
        }

        btCapsuleShape mShape;
    };

    struct CapsuleShape : CollisionShape {

        virtual CollisionShapeInstancePtr create(typename CollisionShapeManager::HandleType shape) override
        {
            return CollisionShapeInstancePtr { new CapsuleShapeInstance(std::move(shape)) };
        }
    };

    struct SphereShapeInstance : Serialize::VirtualUnit<SphereShapeInstance, VirtualScope<SphereShapeInstance, CollisionShapeInstance>> {
        SphereShapeInstance(typename CollisionShapeManager::HandleType shape)
            : VirtualUnit(std::move(shape))
            , mShape(0.1f)
        {
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual void destroy() override
        {
            delete this;
        }

        virtual std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter> clone() override
        {
            return std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter> { new SphereShapeInstance(*this) };
        }

        float radius() const
        {
            return mShape.getRadius();
        }

        void setRadius(float r)
        {
            mShape.setImplicitShapeDimensions({ r, 0, 0 });
            mShape.setMargin(r);
        }

        btSphereShape mShape;
    };

    struct SphereShape : CollisionShape {

        virtual CollisionShapeInstancePtr create(typename CollisionShapeManager::HandleType shape) override
        {
            return CollisionShapeInstancePtr { new SphereShapeInstance(std::move(shape)) };
        }
    };

    struct CompoundShapeInstance : Serialize::VirtualUnit<CompoundShapeInstance, VirtualScope<CompoundShapeInstance, CollisionShapeInstance>> {

        using VirtualUnit::VirtualUnit;

        void addShape(CollisionShapeManager::ResourceType *res)
        {
            //TODO
            //Engine::Threading::DataLock lock { Engine::App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().mutex(), Engine::Threading::AccessMode::WRITE };

            CollisionShapeManager::InstanceHandle &instance = mChildren.emplace_back(res);
            mShape.addChildShape(btTransform { btQuaternion { 0, 0, 0 } }, instance->get());
        }

        struct CompoundShapeElement : SerializableDataUnit {
            Vector3 mPos;
            Quaternion mOrientation;
            CollisionShapeManager::InstanceHandle mShape;

            CollisionShapeInstance *shapeInstance()
            {
                return mShape;
            }
        };

        std::vector<CompoundShapeElement> shapes() const
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

        void setShapes(std::vector<CompoundShapeElement> shapes)
        {
            mChildren.clear();
            while (mShape.getNumChildShapes() > 0)
                mShape.removeChildShapeByIndex(0);
            for (CompoundShapeElement &element : shapes) {
                auto &instance = mChildren.emplace_back(std::move(element.mShape));
                mShape.addChildShape(btTransform {
                                         btQuaternion { element.mOrientation.v.x, element.mOrientation.v.y, element.mOrientation.v.z, element.mOrientation.w }, btVector3 { element.mPos.x, element.mPos.y, element.mPos.z } },
                    instance->get());
            }
        }

        virtual btCollisionShape *get() override
        {
            return &mShape;
        }

        virtual void destroy() override
        {
            delete this;
        }

        virtual std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter> clone() override
        {
            //TODO
            return std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter> { new CompoundShapeInstance(*this) };
        }

        std::vector<CollisionShapeManager::InstanceHandle> mChildren;
        btCompoundShape mShape;
    };

    struct CompoundShape : CollisionShape {
        virtual CollisionShapeInstancePtr create(typename CollisionShapeManager::HandleType shape) override
        {
            return CollisionShapeInstancePtr { new CompoundShapeInstance(std::move(shape)) };
        }
    };

    CollisionShapeManager::InstanceHandle::InstanceHandle(typename CollisionShapeManager::HandleType shape)
        : mInstance(shape.getUnsafe()->get()->create(std::move(shape)))
    {
    }

    CollisionShapeManager::InstanceHandle::InstanceHandle(typename CollisionShapeManager::ResourceType *res)
        : InstanceHandle(res->loadData())
    {
    }

    CollisionShapeManager::InstanceHandle::InstanceHandle(const InstanceHandle &other)
        : mInstance(other.mInstance ? other.mInstance->clone() : nullptr)
    {
    }

    CollisionShapeManager::InstanceHandle &CollisionShapeManager::InstanceHandle::operator=(const InstanceHandle &other)
    {
        mInstance = other.mInstance ? other.mInstance->clone() : nullptr;
        return *this;
    }

    CollisionShapeManager::InstanceHandle &CollisionShapeManager::InstanceHandle::operator=(InstanceHandle &&other)
    {
        std::swap(mInstance, other.mInstance);
        return *this;
    }

    void CollisionShapeManager::InstanceHandle::load(std::string_view name, CollisionShapeManager *loader)
    {
        *this = CollisionShapeManager::load(name, loader);
    }

    void CollisionShapeManager::InstanceHandle::reset()
    {
        mInstance.reset();
    }

    CollisionShapeManager::ResourceType *CollisionShapeManager::InstanceHandle::resource() const
    {
        return mInstance ? mInstance->resource() : nullptr;
    }

    CollisionShapeInstance *CollisionShapeManager::InstanceHandle::operator->() const
    {
        return mInstance.get();
    }

    CollisionShapeManager::InstanceHandle::operator CollisionShapeInstance *() const
    {
        return mInstance.get();
    }

    Serialize::StreamResult CollisionShapeManager::InstanceHandle::readState(Serialize::SerializeInStream &in, const char *name)
    {
        std::string resName;
        STREAM_PROPAGATE_ERROR(in.format().beginExtended(in, name, 1));
        STREAM_PROPAGATE_ERROR(Serialize::read(in, resName, "name"));
        if (resName.empty()) {
            reset();
            STREAM_PROPAGATE_ERROR(in.format().beginCompound(in, name));
            STREAM_PROPAGATE_ERROR(in.format().endCompound(in, name));
        } else {
            load(resName);
            assert(mInstance);
            STREAM_PROPAGATE_ERROR(Serialize::read(in, *mInstance, name));
        }
        return {};
    }

    void CollisionShapeManager::InstanceHandle::writeState(Serialize::SerializeOutStream &out, const char *name) const
    {
        bool hasShape = static_cast<bool>(mInstance);
        std::string_view resName = hasShape ? mInstance->resource()->name() : "";
        out.format().beginExtended(out, name, 1);
        Serialize::write(out, resName, "name");
        if (!hasShape) {
            out.format().beginCompound(out, name);
            out.format().endCompound(out, name);
        } else {
            Serialize::write(out, *mInstance, name);
        }
    }

    CollisionShapeManager::ResourceType *CollisionShapeInstance::resource() const
    {
        return mHandle.resource();
    }

    bool CollisionShapeInstance::available() const
    {
        return mHandle.available();
    }

    CollisionShapeManager::CollisionShapeManager()
        : ResourceLoader({ ".fbx", ".dae" })
    {
        getOrCreateManual(
            "Cube", {}, [](CollisionShapeManager *mgr, std::unique_ptr<CollisionShape> &data, ResourceDataInfo &info) {
                data = std::make_unique<BoxShape>();
                return Threading::make_ready_task(true);
            },
            {}, this);

        getOrCreateManual(
            "Plane", {}, [](CollisionShapeManager *mgr, std::unique_ptr<CollisionShape> &data, ResourceDataInfo &info) {
                data = std::make_unique<PlaneShape>();
                return Threading::make_ready_task(true);
            },
            {}, this);

        getOrCreateManual(
            "Capsule", {}, [](CollisionShapeManager *mgr, std::unique_ptr<CollisionShape> &data, ResourceDataInfo &info) {
                data = std::make_unique<CapsuleShape>();
                return Threading::make_ready_task(true);
            },
            {}, this);

        getOrCreateManual(
            "Sphere", {}, [](CollisionShapeManager *mgr, std::unique_ptr<CollisionShape> &data, ResourceDataInfo &info) {
                data = std::make_unique<SphereShape>();
                return Threading::make_ready_task(true);
            },
            {}, this);

        getOrCreateManual(
            "Compound", {}, [](CollisionShapeManager *mgr, std::unique_ptr<CollisionShape> &data, ResourceDataInfo &info) {
                data = std::make_unique<CompoundShape>();
                return Threading::make_ready_task(true);
            },
            {}, this);
    }

    Threading::ImmediateTask<bool> CollisionShapeManager::loadImpl(std::unique_ptr<CollisionShape> &shape, ResourceDataInfo &info)
    {
        std::unique_ptr<MeshShape> meshShapePtr = std::make_unique<MeshShape>();
        MeshShape *meshShape = meshShapePtr.get();
        shape = std::move(meshShapePtr);

        Render::MeshLoader::HandleType mesh;
        if (!co_await mesh.load(info.resource()->name()))
            co_return false;

        meshShape->setMesh(mesh->mVertices, mesh->mVertexSize);

        co_return true;
    }

    void CollisionShapeManager::unloadImpl(std::unique_ptr<CollisionShape> &shape, ResourceDataInfo &info)
    {
    }

    void CollisionShapeInstanceDeleter::operator()(CollisionShapeInstance *instance)
    {
        instance->destroy();
    }

    CollisionShapeInstance::CollisionShapeInstance(typename CollisionShapeManager::HandleType shape)
        : mHandle(std::move(shape))
    {
    }

    CollisionShapeInstance::~CollisionShapeInstance()
    {
    }

}
}

METATABLE_BEGIN_BASE(Engine::Physics::MeshShape, Engine::Physics::CollisionShapeInstance)
METATABLE_END(Engine::Physics::MeshShape)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::MeshShape, Engine::Physics::CollisionShapeInstance)
SERIALIZETABLE_END(Engine::Physics::MeshShape)

METATABLE_BEGIN_BASE(Engine::Physics::BoxShapeInstance, Engine::Physics::CollisionShapeInstance)
PROPERTY(halfExtends, getHalfExtends, setHalfExtends)
METATABLE_END(Engine::Physics::BoxShapeInstance)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::BoxShapeInstance, Engine::Physics::CollisionShapeInstance)
ENCAPSULATED_FIELD(halfExtends, getHalfExtends, setHalfExtends)
SERIALIZETABLE_END(Engine::Physics::BoxShapeInstance)

METATABLE_BEGIN_BASE(Engine::Physics::PlaneShapeInstance, Engine::Physics::CollisionShapeInstance)
METATABLE_END(Engine::Physics::PlaneShapeInstance)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::PlaneShapeInstance, Engine::Physics::CollisionShapeInstance)
SERIALIZETABLE_END(Engine::Physics::PlaneShapeInstance)

METATABLE_BEGIN_BASE(Engine::Physics::CapsuleShapeInstance, Engine::Physics::CollisionShapeInstance)
METATABLE_END(Engine::Physics::CapsuleShapeInstance)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::CapsuleShapeInstance, Engine::Physics::CollisionShapeInstance)
SERIALIZETABLE_END(Engine::Physics::CapsuleShapeInstance)

METATABLE_BEGIN_BASE(Engine::Physics::SphereShapeInstance, Engine::Physics::CollisionShapeInstance)
PROPERTY(radius, radius, setRadius)
METATABLE_END(Engine::Physics::SphereShapeInstance)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::SphereShapeInstance, Engine::Physics::CollisionShapeInstance)
ENCAPSULATED_FIELD(radius, radius, setRadius)
SERIALIZETABLE_END(Engine::Physics::SphereShapeInstance)

METATABLE_BEGIN_BASE(Engine::Physics::CompoundShapeInstance, Engine::Physics::CollisionShapeInstance)
PROPERTY(Shapes, shapes, setShapes)
FUNCTION(addShape, Shape)
METATABLE_END(Engine::Physics::CompoundShapeInstance)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::CompoundShapeInstance, Engine::Physics::CollisionShapeInstance)
ENCAPSULATED_FIELD(Shapes, shapes, setShapes)
SERIALIZETABLE_END(Engine::Physics::CompoundShapeInstance)

METATABLE_BEGIN(Engine::Physics::CompoundShapeInstance::CompoundShapeElement)
MEMBER(mPos)
MEMBER(mOrientation)
READONLY_PROPERTY(Shape, shapeInstance)
METATABLE_END(Engine::Physics::CompoundShapeInstance::CompoundShapeElement)

SERIALIZETABLE_BEGIN(Engine::Physics::CompoundShapeInstance::CompoundShapeElement)
FIELD(mPos)
FIELD(mOrientation)
FIELD(mShape)
SERIALIZETABLE_END(Engine::Physics::CompoundShapeInstance::CompoundShapeElement)