#pragma once

#include "bullet3-2.89/src/BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "rigidbody.h"

namespace Engine {
namespace Physics {

    struct RigidBodyImpl : RigidBody, btMotionState {
        RigidBodyImpl(const ObjectPtr &data)
            : RigidBody(data)
            , mRigidBody(btRigidBody::btRigidBodyConstructionInfo { 0.0f, this, nullptr, { 0.0f, 0.0f, 0.0f } })
        {
            mRigidBody.setUserPointer(static_cast<RigidBody *>(this));
        }

        virtual void setWorldTransform(const btTransform &transform) override
        {
            if (mTransform) {
                Matrix4 p = mTransform->parentMatrix();

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
                Matrix4 m = mTransform->worldMatrix();

                Vector3 pos;
                Vector3 scale;
                Quaternion orientation;
                std::tie(pos, scale, orientation) = DecomposeTransformMatrix(m);
                transform = btTransform { { orientation.v.x, orientation.v.y, orientation.v.z, orientation.w }, { pos.x, pos.y, pos.z } };
            } else {
                transform = btTransform { { 0.0f, 0.0f, 0.0f, 1.0f } };
            }
        }

        btRigidBody mRigidBody;
    };

}
}

RegisterType(Engine::Physics::RigidBodyImpl);
RegisterType(Engine::Scene::Entity::EntityComponentList<Engine::Physics::RigidBodyImpl>);