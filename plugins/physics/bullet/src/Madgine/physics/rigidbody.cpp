#include "../bulletlib.h"

#include "physicsmanager.h"

#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/scenemanager.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/scene/entity/entity.h"

#include "Meta/math/transformation.h"

#include "rigidbodyimpl.h"


ENTITYCOMPONENT_IMPL_EX(RigidBody, Engine::Physics::RigidBody, Engine::Physics::RigidBodyImpl)

METATABLE_BEGIN(Engine::Physics::RigidBody)
PROPERTY(Mass, mass, setMass)
PROPERTY(Friction, friction, setFriction)
PROPERTY(Kinematic, kinematic, setKinematic)
PROPERTY(LinearFactor, linearFactor, setLinearFactor)
PROPERTY(AngularFactor, angularFactor, setAngularFactor)
PROPERTY(Shape, getShape, setShape)
READONLY_PROPERTY(ShapeData, getShapeInstance)
METATABLE_END(Engine::Physics::RigidBody)

SERIALIZETABLE_BEGIN(Engine::Physics::RigidBody)
FIELD(mShapeHandle)
ENCAPSULATED_FIELD(Kinematic, kinematic, setKinematic)
ENCAPSULATED_FIELD(Mass, mass, setMass)
ENCAPSULATED_FIELD(Friction, friction, setFriction)
ENCAPSULATED_FIELD(LinearFactor, linearFactor, setLinearFactor)
ENCAPSULATED_FIELD(AngularFactor, angularFactor, setAngularFactor)
SERIALIZETABLE_END(Engine::Physics::RigidBody)

METATABLE_BEGIN_BASE(Engine::Physics::RigidBodyImpl, Engine::Physics::RigidBody)
METATABLE_END(Engine::Physics::RigidBodyImpl)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Physics::RigidBodyImpl, Engine::Physics::RigidBody)
SERIALIZETABLE_END(Engine::Physics::RigidBodyImpl)

namespace Engine {
namespace Physics {

    RigidBody::RigidBody(const ObjectPtr &data)
        : NamedUniqueComponent(data)

    {
    }

    RigidBody::RigidBody(RigidBody &&other) = default;

    RigidBody::~RigidBody() = default;

    RigidBody &RigidBody::operator=(RigidBody &&) = default;

    void RigidBody::init(Scene::Entity::Entity *entity)
    {
        if (!mShapeHandle)
            mShapeHandle.load("Cube");

        mTransform = entity->addComponent<Scene::Entity::Transform>();
        mSceneMgr = &entity->sceneMgr();

        Matrix4 m = mTransform->worldMatrix();

        Vector3 pos;
        Vector3 scale;
        Quaternion orientation;
        std::tie(pos, scale, orientation) = DecomposeTransformMatrix(m);
        get()->setWorldTransform(btTransform { { orientation.v.x, orientation.v.y, orientation.v.z, orientation.w }, { pos.x, pos.y, pos.z } });

        if (mShapeHandle)
            get()->setCollisionShape(mShapeHandle->get());

        add();
    }

    void RigidBody::finalize(Scene::Entity::Entity *entity)
    {
        remove();
    }

    btRigidBody *RigidBody::get()
    {
        return &static_cast<RigidBodyImpl *>(this)->mRigidBody;
    }

    const btRigidBody *RigidBody::get() const
    {
        return &static_cast<const RigidBodyImpl *>(this)->mRigidBody;
    }

    void RigidBody::activate()
    {
        get()->activate(true);
    }

    Scene::Entity::Transform *RigidBody::transform()
    {
        return mTransform;
    }

    float RigidBody::mass() const
    {
        return get()->getMass();
    }

    void RigidBody::setMass(float mass)
    {
        float oldMass = get()->getMass();
        if (mass != oldMass) {
            if (mTransform)
                remove();
            btVector3 inertia;
            mShapeHandle->get()->calculateLocalInertia(mass, inertia);
            get()->setMassProps(mass, inertia);
            if (mTransform) {
                add();
                get()->activate(true);
                if (oldMass == 0.0f) {
                    Engine::Scene::Entity::Transform *component = mTransform;
                    if (component) {
                        const Vector3 &pos = component->getPosition();
                        const Quaternion &orientation = component->getOrientation();
                        get()->setWorldTransform(btTransform { { orientation.v.x, orientation.v.y, orientation.v.z, orientation.w }, { pos.x, pos.y, pos.z } });
                    }
                    get()->clearForces();
                }
            }
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
                get()->activate(true);
                add();
            }
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

    void RigidBody::setVelocity(const Vector3 &v)
    {
        get()->setLinearVelocity({ v.x, v.y, v.z });
    }

    void RigidBody::setShape(typename CollisionShapeManager::HandleType handle)
    {
        mShapeHandle = std::move(handle);

        get()->setCollisionShape(mShapeHandle->get());
    }

    void RigidBody::setShapeName(std::string_view name)
    {
        mShapeHandle.load(name);

        get()->setCollisionShape(mShapeHandle->get());
    }

    CollisionShapeManager::ResourceType *RigidBody::getShape() const
    {
        return mShapeHandle ? mShapeHandle->mShape.resource() : nullptr;
    }

    CollisionShapeInstance *RigidBody::getShapeInstance() const
    {
        return mShapeHandle;
    }

    void RigidBody::add()
    {
        mSceneMgr->getComponent<PhysicsManager>().world().addRigidBody(get());
    }

    void RigidBody::remove()
    {
        mSceneMgr->getComponent<PhysicsManager>().world().removeRigidBody(get());
    }

    Scene::SceneManager *RigidBody::sceneMgr()
    {
        return mSceneMgr;
    }

}
}
