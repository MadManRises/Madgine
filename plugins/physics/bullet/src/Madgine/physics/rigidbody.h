#pragma once

#include "Madgine/scene/entity/entitycomponent.h"

#include "bullet3-2.89/src/btBulletDynamicsCommon.h"

#include "Madgine/scene/entity/entityptr.h"

namespace Engine {
namespace Physics {

    struct MADGINE_BULLET_EXPORT RigidBody : Engine::Scene::Entity::EntityComponent<RigidBody>, btMotionState {

        RigidBody(const Engine::ObjectPtr &data = {});

        virtual void init(const Scene::Entity::EntityPtr &entity) override;
        virtual void finalize(const Scene::Entity::EntityPtr &entity) override;

        virtual void setWorldTransform(const btTransform &transform) override;
        virtual void getWorldTransform(btTransform &transform) const override;

        btRigidBody *get();

        float getMass() const;
        void setMass(float mass);

    private:
        btBoxShape mShape;
        btRigidBody mRigidBody;
        Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> mTransform;
    };

}
}

RegisterType(Engine::Physics::RigidBody);