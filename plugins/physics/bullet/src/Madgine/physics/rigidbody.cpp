#include "../bulletlib.h"

#include "rigidbody.h"

#include "physicsmanager.h"

#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/scenemanager.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Madgine/scene/entity/entity.h"

#include "Modules/math/transformation.h"

ENTITYCOMPONENT_IMPL(RigidBody, Engine::Physics::RigidBody)

METATABLE_BEGIN(Engine::Physics::RigidBody)
PROPERTY(Mass, mass, setMass)
PROPERTY(Kinematic, kinematic, setKinematic)
METATABLE_END(Engine::Physics::RigidBody)

SERIALIZETABLE_BEGIN(Engine::Physics::RigidBody)
SERIALIZETABLE_END(Engine::Physics::RigidBody)

namespace Engine {
namespace Physics {

    struct RigidBody::Data : btMotionState {

        Data(Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> transform)
            : mTransform(std::move(transform))
            , mShape({ 0.5f, 0.5f, 0.5f })
            , mRigidBody(btRigidBody::btRigidBodyConstructionInfo { 0.0f, this, &mShape, { 0.0f, 0.0f, 0.0f } })
        {
        }

        virtual void setWorldTransform(const btTransform &transform) override
        {
            if (mTransform) {
                Matrix4 p = mTransform.parentMatrix();

                Matrix4 m = p.Inverse() * TransformMatrix(Vector3 { transform.getOrigin() }, Vector3::UNIT_SCALE, Quaternion { transform.getRotation() });

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
                Matrix4 m = mTransform.worldMatrix();

                Vector3 pos;
                Vector3 scale;
                Quaternion orientation;
                std::tie(pos, scale, orientation) = DecomposeTransformMatrix(m);
                transform = btTransform { { orientation.v.x, orientation.v.y, orientation.v.z, orientation.w }, { pos.x, pos.y, pos.z } };
            } else {
                transform = btTransform { { 0.0f, 0.0f, 0.0f, 1.0f } };
            }
        }

        Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> mTransform;
        btBoxShape mShape;
        btRigidBody mRigidBody;
    };

    RigidBody::RigidBody(const Engine::ObjectPtr &data)
        : EntityComponent(data)
    {
    }

    RigidBody::RigidBody(RigidBody &&other) noexcept = default;

    RigidBody::~RigidBody() = default;

    RigidBody &RigidBody::operator=(RigidBody &&) = default;

    void RigidBody::init(const Scene::Entity::EntityPtr &entity)
    {
        EntityComponent::init(entity);

        assert(!mData);

        mData = std::make_unique<Data>(entity.getComponent<Scene::Entity::Transform>());

        entity->sceneMgr().getComponent<PhysicsManager>().world().addRigidBody(&mData->mRigidBody);
    }

    void RigidBody::finalize(const Scene::Entity::EntityPtr &entity)
    {
        assert(mData);

        entity->sceneMgr().getComponent<PhysicsManager>().world().removeRigidBody(&mData->mRigidBody);

        mData.reset();

        EntityComponent::finalize(entity);
    }

    btRigidBody *RigidBody::get()
    {
        return &mData->mRigidBody;
    }

    float RigidBody::mass() const
    {
        return mData->mRigidBody.getMass();
    }

    void RigidBody::setMass(float mass)
    {
        float oldMass = mData->mRigidBody.getMass();
        if (mass != oldMass) {
            mData->mRigidBody.setMassProps(mass, { 0.0f, 0.0f, 0.0f });
            mData->mTransform.entity()->sceneMgr().getComponent<PhysicsManager>().world().removeRigidBody(&mData->mRigidBody);
            mData->mTransform.entity()->sceneMgr().getComponent<PhysicsManager>().world().addRigidBody(&mData->mRigidBody);
            mData->mRigidBody.activate(true);
            if (oldMass == 0.0f) {
                Engine::Scene::Entity::Transform *component = mData->mTransform;
                if (component) {
                    const Vector3 &pos = component->getPosition();
                    const Quaternion &orientation = component->getOrientation();
                    mData->mRigidBody.setWorldTransform(btTransform { { orientation.v.x, orientation.v.y, orientation.v.z, orientation.w }, { pos.x, pos.y, pos.z } });
                }
                mData->mRigidBody.clearForces();
            }
        }
    }

    void RigidBody::update()
    {
        mData->mTransform.update();
    }

    bool Engine::Physics::RigidBody::kinematic() const
    {
        return mData->mRigidBody.getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT;
    }

    void Engine::Physics::RigidBody::setKinematic(bool kinematic)
    {
        if (kinematic) {
            mData->mRigidBody.setCollisionFlags(mData->mRigidBody.getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            mData->mRigidBody.setActivationState(DISABLE_DEACTIVATION);
        } else {
            mData->mRigidBody.setCollisionFlags(mData->mRigidBody.getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
            mData->mRigidBody.setActivationState(ACTIVE_TAG);
        }
    }

}
}
