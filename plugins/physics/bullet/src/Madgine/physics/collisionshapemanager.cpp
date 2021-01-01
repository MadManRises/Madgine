#include "../bulletlib.h"

#include "collisionshapemanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "meshloader.h"

#include "Modules/math/quaternion.h"

#include "Modules/serialize/streams/operations.h"

#include "Madgine/app/application.h"
#include "Modules/threading/datamutex.h"
#include "Madgine/scene/scenemanager.h"

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

SERIALIZETABLE_BEGIN(Engine::Physics::CollisionShapeInstance)
SERIALIZETABLE_END(Engine::Physics::CollisionShapeInstance)

namespace Engine {
namespace Physics {

    struct MeshShape : CollisionShape, Serialize::VirtualUnit<MeshShape, VirtualScope<MeshShape, CollisionShapeInstance>> {

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

        virtual MeshShape *clone() override
        {
            return this;
        }

        virtual bool isInstance() override
        {
            return false;
        }

        btConvexHullShape mShape;
    };

    struct BoxShapeInstance : Serialize::VirtualUnit<BoxShapeInstance, VirtualScope<BoxShapeInstance, CollisionShapeInstance>> {
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

        virtual BoxShapeInstance *clone() override
        {
            return new BoxShapeInstance(*this);
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

    struct CompoundShapeInstance : Serialize::VirtualUnit<CompoundShapeInstance, VirtualScope<CompoundShapeInstance, CollisionShapeInstance>> {

        void addShape(CollisionShapeManager::ResourceType *res)
        {
            //TODO
            //Engine::Threading::DataLock lock { Engine::App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().mutex(), Engine::Threading::AccessMode::WRITE };
            
            CollisionShapeManager::InstanceHandle &instance = mChildren.emplace_back(res);
            mShape.addChildShape(btTransform { btQuaternion { 0, 0, 0 } }, instance->get());
        }

        struct CompoundShapeElement : ScopeBase, SerializableUnitBase {
            Vector3 mPos;
            Quaternion mOrientation;
            CollisionShapeManager::InstanceHandle mShape;

            CollisionShapeInstance* shapeInstance() {
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

        virtual bool isInstance() override
        {
            return true;
        }

        virtual CompoundShapeInstance *clone() override
        {
            //TODO
            return new CompoundShapeInstance(*this);
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

    CollisionShapeManager::InstanceHandle::InstanceHandle(const InstanceHandle &other)
        : mInstance(other.mInstance ? other.mInstance->clone() : nullptr)
    {
    }

    CollisionShapeManager::InstanceHandle::InstanceHandle(InstanceHandle &&other) noexcept
        : mInstance(std::exchange(other.mInstance, nullptr))
    {
    }

    CollisionShapeManager::InstanceHandle::~InstanceHandle()
    {
        reset();
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

    void CollisionShapeManager::InstanceHandle::load(const std::string_view &name, CollisionShapeManager *loader)
    {
        *this = CollisionShapeManager::load(name, loader);
    }

    void CollisionShapeManager::InstanceHandle::reset()
    {
        if (mInstance && mInstance->isInstance())
            delete mInstance;
        mInstance = nullptr;
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

    void CollisionShapeManager::InstanceHandle::readState(Serialize::SerializeInStream &in, const char *name)
    {
        std::string resName;
        in.format().beginExtended(in, name, 1);
        Serialize::read(in, resName, "name");
        if (resName.empty()) {
            reset();
            in.format().beginCompound(in, name);
            in.format().endCompound(in, name);
        } else {
            load(resName);
            assert(mInstance);
            Serialize::read(in, *mInstance, name);
        }
    }

    void CollisionShapeManager::InstanceHandle::writeState(Serialize::SerializeOutStream &out, const char *name) const
    {
        bool hasShape = mInstance;
        std::string_view resName = hasShape ? mInstance->mShape.resource()->name() : "";
        out.format().beginExtended(out, name, 1);
        Serialize::write(out, resName, "name");
        if (!hasShape) {
            out.format().beginCompound(out, name);
            out.format().endCompound(out, name);
        } else {
            Serialize::write(out, *mInstance, name);
        }
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

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::MeshShape, Engine::Physics::CollisionShapeInstance)
SERIALIZETABLE_END(Engine::Physics::MeshShape)

METATABLE_BEGIN_BASE(Engine::Physics::BoxShapeInstance, Engine::Physics::CollisionShapeInstance)
PROPERTY(halfExtends, getHalfExtends, setHalfExtends)
METATABLE_END(Engine::Physics::BoxShapeInstance)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::BoxShapeInstance, Engine::Physics::CollisionShapeInstance)
ENCAPSULATED_FIELD(halfExtends, getHalfExtends, setHalfExtends)
SERIALIZETABLE_END(Engine::Physics::BoxShapeInstance)

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