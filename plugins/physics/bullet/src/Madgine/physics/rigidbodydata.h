#pragma once

#include "rigidbody.h"

#include "bullet3-3.24/src/BulletSoftBody/btSoftRigidDynamicsWorld.h"

namespace Engine {
namespace Physics {

    struct RigidBody::Data : btMotionState {
        Data(RigidBody *component, Scene::Entity::Entity *entity = nullptr, Scene::Entity::Transform *transform = nullptr);

        virtual void setWorldTransform(const btTransform &transform) override;
        virtual void getWorldTransform(btTransform &transform) const override;

        uint16_t collisionGroup() const;
        void setCollisionGroup(uint16_t group);

        uint16_t collisionMask() const;
        void setCollisionMask(uint16_t mask);

        void add();
        void remove();

        uint16_t mCollisionGroup = 2;
        uint16_t mCollisionMask = 65533;

        Scene::Entity::Entity *mEntity = nullptr;
        Scene::Entity::Transform *mTransform = nullptr;
        PhysicsManager *mMgr = nullptr;

        btRigidBody mRigidBody;
    };

}
}