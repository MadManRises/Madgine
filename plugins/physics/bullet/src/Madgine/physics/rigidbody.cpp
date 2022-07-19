#include "../bulletlib.h"

#include "physicsmanager.h"

#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/scenemanager.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/scene/entity/entity.h"

#include "Meta/math/transformation.h"

#include "rigidbody.h"

#include "bullet3-2.89/src/BulletSoftBody/btSoftRigidDynamicsWorld.h"

ENTITYCOMPONENT_IMPL(RigidBody, Engine::Physics::RigidBody)

METATABLE_BEGIN(Engine::Physics::RigidBody)
PROPERTY(Mass, mass, setMass)
PROPERTY(Friction, friction, setFriction)
PROPERTY(Kinematic, kinematic, setKinematic)
PROPERTY(LinearFactor, linearFactor, setLinearFactor)
PROPERTY(AngularFactor, angularFactor, setAngularFactor)
PROPERTY(Shape, getShape, setShape)
READONLY_PROPERTY(ShapeData, getShapeInstance)
PROPERTY(Ghost, ghost, setGhost)
PROPERTY(CollisionGroup, collisionGroup, setCollisionGroup)
PROPERTY(CollisionMask, collisionMask, setCollisionMask)
METATABLE_END(Engine::Physics::RigidBody)

SERIALIZETABLE_BEGIN(Engine::Physics::RigidBody)
FIELD(mShapeHandle)
ENCAPSULATED_FIELD(Kinematic, kinematic, setKinematic)
ENCAPSULATED_FIELD(Mass, mass, setMass)
ENCAPSULATED_FIELD(Friction, friction, setFriction)
ENCAPSULATED_FIELD(LinearFactor, linearFactor, setLinearFactor)
ENCAPSULATED_FIELD(AngularFactor, angularFactor, setAngularFactor)
ENCAPSULATED_FIELD(Ghost, ghost, setGhost)
ENCAPSULATED_FIELD(CollisionGroup, collisionGroup, setCollisionGroup)
ENCAPSULATED_FIELD(CollisionMask, collisionMask, setCollisionMask)
SERIALIZETABLE_END(Engine::Physics::RigidBody)

namespace Engine {
namespace Physics {

    struct RigidBody::Data : btMotionState {
        Data(RigidBody *component, Scene::Entity::Entity *entity = nullptr, Scene::Entity::Transform *transform = nullptr)
            : mRigidBody(btRigidBody::btRigidBodyConstructionInfo { 0.0f, this, nullptr, { 0.0f, 0.0f, 0.0f } })
            , mEntity(entity)
            , mTransform(transform)
        {
            mRigidBody.setUserPointer(component);
            mRigidBody.setUserIndex(65533);
            mRigidBody.setUserIndex2(2);
        }

        virtual void setWorldTransform(const btTransform &transform) override
        {
            if (mTransform) {
                Matrix4 p = mTransform->parentMatrix();

                btQuaternion q = transform.getRotation();

                Matrix4 m = p.Inverse() * TransformMatrix(Vector3 { transform.getOrigin() }, Vector3::UNIT_SCALE, Quaternion { q.x(), q.y(), q.z(), q.w() });

                Vector3 pos;
                Vector3 scale;
                Quaternion orientation;
                std::tie(pos, scale, orientation) = DecomposeTransformMatrix(m);

                mTransform->setPosition(pos);
                mTransform->setOrientation(orientation);
            }
        }

        virtual void getWorldTransform(btTransform &transform) const override
        {
            if (mTransform) {
                Matrix4 m = mTransform->worldMatrix();

                Vector3 pos;
                Vector3 scale;
                Quaternion orientation;
                std::tie(pos, scale, orientation) = DecomposeTransformMatrix(m);
                transform = btTransform { { orientation.x, orientation.y, orientation.z, orientation.w }, { pos.x, pos.y, pos.z } };
            } else {
                transform = btTransform { { 0.0f, 0.0f, 0.0f, 1.0f } };
            }
        }

        Scene::Entity::Entity *mEntity = nullptr;
        Scene::Entity::Transform *mTransform = nullptr;

        btRigidBody mRigidBody;
    };

    RigidBody::RigidBody(const ObjectPtr &data)
        : NamedComponent(data)

    {
        mData = std::make_unique<Data>(this);
    }

    RigidBody::RigidBody(RigidBody &&other)
        : NamedComponent(std::move(other))
        , mShapeHandle(std::move(other.mShapeHandle))
        , mMgr(other.mMgr)
        , mData(std::move(other.mData))
    {
        if (mData)
            get()->setUserPointer(this);
    }

    RigidBody::~RigidBody() = default;

    RigidBody &RigidBody::operator=(RigidBody &&other)
    {
        NamedComponent::operator=(std::move(other));
        std::swap(mShapeHandle, other.mShapeHandle);
        mMgr = other.mMgr;
        std::swap(mData, other.mData);
        if (mData)
            get()->setUserPointer(this);
        return *this;
    }

    void RigidBody::init(Scene::Entity::Entity *entity)
    {
        Scene::Entity::Transform *transform = entity->addComponent<Scene::Entity::Transform>();

        mData->mEntity = entity;
        mData->mTransform = transform;

        mMgr = &entity->sceneMgr().getComponent<PhysicsManager>();

        Matrix4 m = transform->worldMatrix();

        const auto &[pos, scale, orientation] = DecomposeTransformMatrix(m);
        get()->setWorldTransform(btTransform { { orientation.x, orientation.y, orientation.z, orientation.w }, { pos.x, pos.y, pos.z } });

        if (mShapeHandle && mShapeHandle->available())
            get()->setCollisionShape(mShapeHandle->get());

        add();
    }

    void RigidBody::finalize(Scene::Entity::Entity *entity)
    {
        remove();
    }

    void RigidBody::update()
    {
        if (mShapeHandle && mShapeHandle->available() && !get()->getCollisionShape()) {
            get()->setCollisionShape(mShapeHandle->get());
            float mass = get()->getMass();
            btVector3 inertia;
            mShapeHandle->get()->calculateLocalInertia(mass, inertia);
            get()->setMassProps(mass, inertia);
            add();
        }
    }

    btRigidBody *RigidBody::get()
    {
        return &mData->mRigidBody;
    }

    const btRigidBody *RigidBody::get() const
    {
        return &mData->mRigidBody;
    }

    void RigidBody::activate()
    {
        get()->activate(true);
    }

    Scene::Entity::EntityPtr RigidBody::entity()
    {
        return mData->mEntity;
    }

    Scene::Entity::Transform *RigidBody::transform()
    {
        return mData->mTransform;
    }

    float RigidBody::mass() const
    {
        return get()->getMass();
    }

    void RigidBody::setMass(float mass)
    {
        float oldMass = get()->getMass();
        if (mass != oldMass) {
            remove();
            btVector3 inertia;
            if (mShapeHandle && mShapeHandle->available()) {
                mShapeHandle->get()->calculateLocalInertia(mass, inertia);
            }
            get()->setMassProps(mass, inertia);
            add();
        }
    }

    bool RigidBody::kinematic() const
    {
        return get()->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT;
    }

    void RigidBody::setKinematic(bool kinematic)
    {
        if (kinematic) {
            get()->setCollisionFlags(get()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            get()->setActivationState(DISABLE_DEACTIVATION);
            get()->setCollisionFlags(get()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        } else {
            get()->setCollisionFlags(get()->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
            get()->forceActivationState(ISLAND_SLEEPING);
            if (get()->getMass() > 0) {
                remove();
                add();
            }
        }
    }

    bool RigidBody::ghost() const
    {
        return get()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE;
    }

    void RigidBody::setGhost(bool ghost)
    {
        if (ghost) {
            get()->setCollisionFlags(get()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        } else {
            get()->setCollisionFlags(get()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }

    float RigidBody::friction() const
    {
        return get()->getFriction();
    }

    void RigidBody::setFriction(float friction)
    {
        get()->setFriction(friction);
    }

    Vector3 RigidBody::linearFactor() const
    {
        return get()->getLinearFactor().m_floats;
    }

    void RigidBody::setLinearFactor(const Vector3 &factor)
    {
        get()->setLinearFactor({ factor.x, factor.y, factor.z });
    }

    Vector3 RigidBody::angularFactor() const
    {
        return get()->getAngularFactor().m_floats;
    }

    void RigidBody::setAngularFactor(const Vector3 &factor)
    {
        get()->setAngularFactor({ factor.x, factor.y, factor.z });
    }

    Vector3 RigidBody::angularVelocity() const
    {
        return get()->getAngularVelocity().m_floats;
    }

    void RigidBody::setAngularVelocity(const Vector3 &v)
    {
        get()->setAngularVelocity({ v.x, v.y, v.z });
    }

    Vector3 RigidBody::velocity() const
    {
        return get()->getLinearVelocity().m_floats;
    }

    void RigidBody::setVelocity(const Vector3 &v)
    {
        get()->setLinearVelocity({ v.x, v.y, v.z });
    }

    void RigidBody::setOrientation(const Quaternion &q)
    {
        btTransform t = get()->getWorldTransform();
        t.setRotation({ q.x, q.y, q.z, q.w });
        get()->setWorldTransform(t);
    }

    uint16_t RigidBody::collisionGroup() const
    {
        return get()->getUserIndex2();
    }

    void RigidBody::setCollisionGroup(uint16_t group)
    {
        if (get()->getBroadphaseProxy())
            get()->getBroadphaseProxy()->m_collisionFilterGroup = group;
        get()->setUserIndex2(group);
    }

    uint16_t RigidBody::collisionMask() const
    {
        return get()->getUserIndex();
    }

    void RigidBody::setCollisionMask(uint16_t mask)
    {
        if (get()->getBroadphaseProxy())
            get()->getBroadphaseProxy()->m_collisionFilterMask = mask;
        get()->setUserIndex(mask);
    }

    void RigidBody::setShape(typename CollisionShapeManager::Handle handle)
    {
        remove();

        mShapeHandle = std::move(handle);
        if (mShapeHandle->available()) {
            get()->setCollisionShape(mShapeHandle->get());
        } else {
            get()->setCollisionShape(nullptr);
        }
        add();
    }

    void RigidBody::setShapeName(std::string_view name)
    {
        remove();

        mShapeHandle.load(name);
        if (mShapeHandle->available()) {
            get()->setCollisionShape(mShapeHandle->get());
        } else {
            get()->setCollisionShape(nullptr);
        }
        add();
    }

    CollisionShapeManager::Resource *RigidBody::getShape() const
    {
        return mShapeHandle ? mShapeHandle.resource() : nullptr;
    }

    CollisionShapeInstance *RigidBody::getShapeInstance() const
    {
        return mShapeHandle;
    }

    void RigidBody::add()
    {
        if (mMgr && get()->getCollisionShape()) {
            mMgr->world().addRigidBody(get(), collisionGroup(), collisionMask());
            get()->activate(true);
            get()->clearForces();
        }
    }

    void RigidBody::remove()
    {
        if (mMgr)
            mMgr->world().removeRigidBody(get());
    }

    PhysicsManager *RigidBody::mgr()
    {
        return mMgr;
    }

}
}
