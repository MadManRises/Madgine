#include "../bulletlib.h"

#include "rigidbody.h"

#include "physicsmanager.h"

#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/scenemanager.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Madgine/scene/entity/entity.h"

ENTITYCOMPONENT_IMPL(RigidBody, Engine::Physics::RigidBody)

METATABLE_BEGIN(Engine::Physics::RigidBody)
PROPERTY(Mass, getMass, setMass)
METATABLE_END(Engine::Physics::RigidBody)

SERIALIZETABLE_BEGIN(Engine::Physics::RigidBody)
SERIALIZETABLE_END(Engine::Physics::RigidBody)

namespace Engine {
namespace Physics {

    RigidBody::RigidBody(const Engine::ObjectPtr &data)
        : EntityComponent(data)
        , mShape({ 0.5f, 0.5f, 0.5f })
        , mRigidBody(btRigidBody::btRigidBodyConstructionInfo { 0.0f, this, &mShape, { 0.0f, 0.0f, 0.0f } })
    {
    }

    void RigidBody::init(const Scene::Entity::EntityPtr &entity)
    {
        EntityComponent::init(entity);

        entity->sceneMgr().getComponent<PhysicsManager>().addRigidBody(this);

        mTransform = entity;
    }

    void RigidBody::finalize(const Scene::Entity::EntityPtr &entity)
    {
        entity->sceneMgr().getComponent<PhysicsManager>().removeRigidBody(this);

        EntityComponent::finalize(entity);
    }

    void RigidBody::setWorldTransform(const btTransform &transform)
    {
        Engine::Scene::Entity::Transform *component = mTransform->getComponent<Engine::Scene::Entity::Transform>();
        if (component) {
            const btVector3 &pos = transform.getOrigin();
            const btQuaternion &orientation = transform.getRotation();
            component->setPosition({ pos.x(), pos.y(), pos.z() });
            component->setOrientation({ orientation.x(), orientation.y(), orientation.z(), orientation.w() });
        }
    }

    void RigidBody::getWorldTransform(btTransform &transform) const
    {
        const Engine::Scene::Entity::Transform *component = mTransform->getComponent<Engine::Scene::Entity::Transform>();
        if (component) {
            const Vector3 &pos = component->getPosition();
            const Quaternion &orientation = component->getOrientation();
            transform = btTransform { { orientation.v.x, orientation.v.y, orientation.v.z, orientation.w }, { pos.x, pos.y, pos.z } };
        } else {
            transform = btTransform { { 0.0f, 0.0f, 0.0f, 1.0f } };
        }
    }

    btRigidBody *RigidBody::get()
    {
        return &mRigidBody;
    }

    float RigidBody::getMass() const
    {
        return mRigidBody.getMass();
    }

    void RigidBody::setMass(float mass)
    {
        float oldMass = mRigidBody.getMass(); 
        if (mass != oldMass) {
            mRigidBody.setMassProps(mass, { 0.0f, 0.0f, 0.0f });
            mTransform->sceneMgr().getComponent<PhysicsManager>().updateRigidBody(this);
            if (oldMass == 0.0f) {
                Engine::Scene::Entity::Transform *component = mTransform->getComponent<Engine::Scene::Entity::Transform>();
                if (component) {
                    const Vector3 &pos = component->getPosition();
                    const Quaternion &orientation = component->getOrientation();
                    mRigidBody.setWorldTransform(btTransform { { orientation.v.x, orientation.v.y, orientation.v.z, orientation.w }, { pos.x, pos.y, pos.z } });
                }
                mRigidBody.clearForces();
            }
        }

    }

}
}
