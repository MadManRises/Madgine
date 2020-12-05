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
PROPERTY(Friction, friction, setFriction)
PROPERTY(Kinematic, kinematic, setKinematic)
PROPERTY(Shape, getShape, setShape)
METATABLE_END(Engine::Physics::RigidBody)

SERIALIZETABLE_BEGIN(Engine::Physics::RigidBody)
SERIALIZETABLE_END(Engine::Physics::RigidBody)

namespace Engine {
namespace Physics {

    struct RigidBody::Data : btMotionState {

        Data(Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> transform, btCollisionShape *shape = nullptr)
            : mTransform(std::move(transform))
            , mRigidBody(btRigidBody::btRigidBodyConstructionInfo { 0.0f, this, shape, { 0.0f, 0.0f, 0.0f } })
        {
            mRigidBody.setAngularFactor({ 0, 0, 1 });
            mRigidBody.setLinearFactor({ 1, 1, 0 });
            //mRigidBody.setFriction(0.01f);
            mRigidBody.setUserPointer(this);
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

        void add()
        {
            mTransform.sceneMgr()->getComponent<PhysicsManager>().world().addRigidBody(&mRigidBody);
        }

        void remove()
        {
            mTransform.sceneMgr()->getComponent<PhysicsManager>().world().removeRigidBody(&mRigidBody);
        }

        Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> mTransform;
        btRigidBody mRigidBody;
    };

    RigidBody::RigidBody(const Engine::ObjectPtr &data)
        : EntityComponent(data)
    {
    }

    RigidBody::RigidBody(RigidBody &&other) = default;

    RigidBody::~RigidBody() = default;

    RigidBody &RigidBody::operator=(RigidBody &&) = default;

    void RigidBody::init(const Scene::Entity::EntityPtr &entity)
    {
        assert(!mData);

        mShapeHandle.load("Cube");

        mData = std::make_unique<Data>(entity.getComponent<Scene::Entity::Transform>(), mShapeHandle ? mShapeHandle->get()->get() : nullptr);

        mData->add();
    }

    void RigidBody::finalize(const Scene::Entity::EntityPtr &entity)
    {
        assert(mData);

        mData->remove();

        mData.reset();        
    }

    btRigidBody *RigidBody::get()
    {
        return &mData->mRigidBody;
    }

    void RigidBody::activate()
    {
        mData->mRigidBody.activate(true);
    }

    const Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> &Engine::Physics::RigidBody::transform()
    {
        return mData->mTransform;
    }

    float RigidBody::mass() const
    {
        return mData->mRigidBody.getMass();
    }

    void RigidBody::setMass(float mass)
    {
        float oldMass = mData->mRigidBody.getMass();
        if (mass != oldMass) {
            mData->remove();
            btVector3 inertia;
            mShapeHandle->get()->get()->calculateLocalInertia(mass, inertia);
            mData->mRigidBody.setMassProps(mass, inertia);
            mData->add();
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

    bool RigidBody::kinematic() const
    {
        return mData->mRigidBody.getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT;
    }

    void RigidBody::setKinematic(bool kinematic)
    {
        if (kinematic) {
            mData->mRigidBody.setCollisionFlags(mData->mRigidBody.getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            mData->mRigidBody.setActivationState(DISABLE_DEACTIVATION);
            mData->mRigidBody.setCollisionFlags(mData->mRigidBody.getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        } else {
            mData->mRigidBody.setCollisionFlags(mData->mRigidBody.getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
            mData->mRigidBody.forceActivationState(ISLAND_SLEEPING);
            if (mData->mRigidBody.getMass() > 0) {
                mData->remove();
                mData->mRigidBody.activate(true);
                mData->add();
            }
        }
    }

    float RigidBody::friction() const
    {
        return mData->mRigidBody.getFriction();
    }

    void RigidBody::setFriction(float friction)
    {
        mData->mRigidBody.setFriction(friction);
    }

    void RigidBody::setShape(typename CollisionShapeManager::HandleType handle)
    {
        mShapeHandle = handle;

        mData->mRigidBody.setCollisionShape(mShapeHandle->get()->get());
    }

    void RigidBody::setShapeName(const std::string_view &name)
    {
        mShapeHandle.load(name);

        mData->mRigidBody.setCollisionShape(mShapeHandle->get()->get());
    }

    CollisionShapeManager::ResourceType *RigidBody::getShape() const
    {
        return mShapeHandle.resource();
    }

    Scene::SceneManager *RigidBody::sceneMgrFromData(Data *data)
    {
        return data->mTransform.sceneMgr();
    }

}
}
