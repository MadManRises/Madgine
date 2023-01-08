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

#include "bullet3-3.24/src/BulletSoftBody/btSoftBody.h"
#include "bullet3-3.24/src/BulletSoftBody/btSoftBodyHelpers.h"
#include "bullet3-3.24/src/BulletSoftBody/btSoftRigidDynamicsWorld.h"

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

    struct SoftBodySkeleton::Data {

        Data(btSoftBodyWorldInfo &info, Scene::Entity::Transform *transform)
            : mTransform(transform)
        // : mSoftBody(btSoftBody btSoftBody::btSoftBodyConstructionInfo { 0.0f, this, nullptr, { 0.0f, 0.0f, 0.0f } })
        {
            mSoftBody = std::unique_ptr<btSoftBody> { btSoftBodyHelpers::CreateRope(info, { 0, 0, 0 }, { 0, 1, 0 }, 5, 0) };
        }

        Scene::Entity::Transform *mTransform = nullptr;
        std::unique_ptr<btSoftBody> mSoftBody;
    };

    SoftBodySkeleton::SoftBodySkeleton(const ObjectPtr &data)
        : NamedComponent(data)
    {
    }

    SoftBodySkeleton::SoftBodySkeleton(SoftBodySkeleton &&other) = default;

    SoftBodySkeleton::~SoftBodySkeleton() = default;

    SoftBodySkeleton &SoftBodySkeleton::operator=(SoftBodySkeleton &&) = default;

    void SoftBodySkeleton::init(Scene::Entity::Entity *entity)
    {
        mMgr = &entity->sceneMgr().getComponent<PhysicsManager>();

        mData = std::make_unique<Data>(mMgr->worldInfo(), entity->addComponent<Scene::Entity::Transform>());

        add();
    }

    void SoftBodySkeleton::finalize(Scene::Entity::Entity *entity)
    {
        remove();
    }

    btSoftBody *SoftBodySkeleton::get()
    {
        return mData->mSoftBody.get();
    }

    void SoftBodySkeleton::activate()
    {
        get()->activate(true);
    }

    void SoftBodySkeleton::add()
    {
        mMgr->world().addSoftBody(get());
    }

    void SoftBodySkeleton::remove()
    {
        mMgr->world().removeSoftBody(get());
    }

    void SoftBodySkeleton::attach(RigidBody *rigidbody, size_t index, const Engine::Vector3 &offset)
    {
        get()->appendAnchor(index, rigidbody->get(), { offset.x, offset.y, offset.z });
    }

}
}
