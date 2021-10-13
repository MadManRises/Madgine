#include "../bulletlib.h"

#include "softbodyskeleton.h"

#include "physicsmanager.h"

#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/scenemanager.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/scene/entity/entity.h"

#include "Meta/math/transformation.h"

#include "rigidbody.h"


#include "bullet3-2.89/src/BulletSoftBody/btSoftBodyHelpers.h"
#include "bullet3-2.89/src/BulletSoftBody/btSoftBody.h"
#include "bullet3-2.89/src/BulletSoftBody/btSoftRigidDynamicsWorld.h"

ENTITYCOMPONENT_IMPL(SoftBodySkeleton, Engine::Physics::SoftBodySkeleton)

METATABLE_BEGIN(Engine::Physics::SoftBodySkeleton)
/* PROPERTY(Mass, mass, setMass)
PROPERTY(Friction, friction, setFriction)
PROPERTY(Kinematic, kinematic, setKinematic)
PROPERTY(LinearFactor, linearFactor, setLinearFactor)
PROPERTY(AngularFactor, angularFactor, setAngularFactor)
PROPERTY(Shape, getShape, setShape)
READONLY_PROPERTY(ShapeData, getShapeInstance)*/
METATABLE_END(Engine::Physics::SoftBodySkeleton)

SERIALIZETABLE_BEGIN(Engine::Physics::SoftBodySkeleton)
/* FIELD(mShapeHandle)
ENCAPSULATED_FIELD(Kinematic, kinematic, setKinematic)
ENCAPSULATED_FIELD(Mass, mass, setMass)
ENCAPSULATED_FIELD(Friction, friction, setFriction)
ENCAPSULATED_FIELD(LinearFactor, linearFactor, setLinearFactor)
ENCAPSULATED_FIELD(AngularFactor, angularFactor, setAngularFactor)*/
SERIALIZETABLE_END(Engine::Physics::SoftBodySkeleton)

namespace Engine {
namespace Physics {

    struct SoftBodySkeleton::Data /* : btMotionState*/ {

        Data(btSoftBodyWorldInfo &info)
           // : mSoftBody(btSoftBody btSoftBody::btSoftBodyConstructionInfo { 0.0f, this, nullptr, { 0.0f, 0.0f, 0.0f } })
        {
            mSoftBody = btSoftBodyHelpers::CreateRope(info, { 0, 0, 0 }, { 0, 1, 0 }, 5, 0);
            //mRigidBody.setFriction(0.01f);
            mSoftBody->setUserPointer(this);            
        }

        /* virtual void setWorldTransform(const btTransform &transform) override
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
        */
        void add()
        {
            mTransform.sceneMgr()->getComponent<PhysicsManager>().world().addSoftBody(mSoftBody);
        }
        /*
        void remove()
        {
            mTransform.sceneMgr()->getComponent<PhysicsManager>().world().removeRigidBody(&mRigidBody);
        }
        */
        Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> mTransform;
        btSoftBody *mSoftBody;
    };

    SoftBodySkeleton::SoftBodySkeleton(const ObjectPtr &data)
        : NamedUniqueComponent(data)
    {
    }

    SoftBodySkeleton::SoftBodySkeleton(SoftBodySkeleton &&other) = default;

    SoftBodySkeleton::~SoftBodySkeleton() = default;

    SoftBodySkeleton &SoftBodySkeleton::operator=(SoftBodySkeleton &&) = default;

    void SoftBodySkeleton::init(Scene::Entity::Entity *entity)
    {
        mData = std::make_unique<Data>(entity->sceneMgr().getComponent<PhysicsManager>().worldInfo());

        /* if (!mShapeHandle)
            mShapeHandle.load("Cube");
            */
        mData->mTransform = entity->addComponent<Scene::Entity::Transform>();
        /*
        Matrix4 m = mData->mTransform.worldMatrix();

        Vector3 pos;
        Vector3 scale;
        Quaternion orientation;
        std::tie(pos, scale, orientation) = DecomposeTransformMatrix(m);
        mData->mRigidBody.setWorldTransform(btTransform { { orientation.v.x, orientation.v.y, orientation.v.z, orientation.w }, { pos.x, pos.y, pos.z } });
        
        if (mShapeHandle)
            mData->mRigidBody.setCollisionShape(mShapeHandle->get());
            */
        mData->add();
    }

    void SoftBodySkeleton::finalize(Scene::Entity::Entity *entity)
    {
        //mData->remove();
    }

    btSoftBody *SoftBodySkeleton::get()
    {
        return mData->mSoftBody;
    }

    void SoftBodySkeleton::activate()
    {
        mData->mSoftBody->activate(true);
    }

    void SoftBodySkeleton::attach(RigidBody* rigidbody, size_t index, const Engine::Vector3 &offset) {
        mData->mSoftBody->appendAnchor(index, rigidbody->get(), { offset.x, offset.y, offset.z });
    }

    /* const Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> &Engine::Physics::RigidBody::transform()
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
            if (mData->mTransform)
                mData->remove();
            btVector3 inertia;
            mShapeHandle->get()->calculateLocalInertia(mass, inertia);
            mData->mRigidBody.setMassProps(mass, inertia);
            if (mData->mTransform) {
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

    Vector3 RigidBody::linearFactor() const
    {
        return mData->mRigidBody.getLinearFactor().m_floats;
    }

    void RigidBody::setLinearFactor(const Vector3 &factor)
    {
        mData->mRigidBody.setLinearFactor({ factor.x, factor.y, factor.z });
    }

    Vector3 RigidBody::angularFactor() const
    {
        return mData->mRigidBody.getAngularFactor().m_floats;
    }

    void RigidBody::setAngularFactor(const Vector3 &factor)
    {
        mData->mRigidBody.setAngularFactor({ factor.x, factor.y, factor.z });
    }

    void RigidBody::setVelocity(const Vector3 &v)
    {
        mData->mRigidBody.setLinearVelocity({ v.x, v.y, v.z });
    }

    void RigidBody::setShape(typename CollisionShapeManager::HandleType handle)
    {
        mShapeHandle = std::move(handle);

        mData->mRigidBody.setCollisionShape(mShapeHandle->get());
    }

    void RigidBody::setShapeName(std::string_view name)
    {
        mShapeHandle.load(name);

        mData->mRigidBody.setCollisionShape(mShapeHandle->get());
    }

    CollisionShapeManager::ResourceType *RigidBody::getShape() const
    {
        return mShapeHandle ? mShapeHandle->mShape.resource() : nullptr;
    }

    CollisionShapeInstance *RigidBody::getShapeInstance() const
    {
        return mShapeHandle;
    }

    Scene::SceneManager *RigidBody::sceneMgrFromData(Data *data)
    {
        return data->mTransform.sceneMgr();
    }*/

}
}
