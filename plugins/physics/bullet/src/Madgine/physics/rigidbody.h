#pragma once

#include "Madgine/scene/entity/entitycomponent.h"

#include "bullet3-2.89/src/btBulletDynamicsCommon.h"

#include "Madgine/scene/entity/entitycomponentptr.h"

#include "Madgine/scene/entity/components/transform.h"

#include "collisionshapemanager.h"

namespace Engine {
namespace Physics {

    struct MADGINE_BULLET_EXPORT RigidBody : Engine::Scene::Entity::EntityComponent<RigidBody> {

        RigidBody(const Engine::ObjectPtr &data = {});
        RigidBody(RigidBody &&other);
        ~RigidBody();

        RigidBody &operator=(RigidBody &&other);

        virtual void init(const Scene::Entity::EntityPtr &entity) override;
        virtual void finalize(const Scene::Entity::EntityPtr &entity) override;

        btRigidBody *get();

        float mass() const;
        void setMass(float mass);

        void update();

        bool kinematic() const;
        void setKinematic(bool kinematic);

        void setShape(typename CollisionShapeManager::HandleType handle);
        CollisionShapeManager::ResourceType *getShape() const;

    private:
        typename CollisionShapeManager::HandleType mShapeHandle;
        struct Data;
        std::unique_ptr<Data> mData;
    };

}
}

RegisterType(Engine::Physics::RigidBody);