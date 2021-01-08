#pragma once

#include "Madgine/scene/entity/entitycomponent.h"

#include "Madgine/scene/entity/entitycomponentptr.h"

#include "Madgine/scene/entity/components/transform.h"

#include "collisionshapemanager.h"

class btRigidBody;

namespace Engine {
namespace Physics {

    struct MADGINE_BULLET_EXPORT RigidBody : Engine::Scene::Entity::EntityComponent<RigidBody> {
        SERIALIZABLEUNIT(RigidBody);

        RigidBody(const Engine::ObjectPtr &data = {});
        RigidBody(RigidBody &&other);
        ~RigidBody();

        RigidBody &operator=(RigidBody &&other);

        void init(Scene::Entity::Entity *entity);
        void finalize(Scene::Entity::Entity *entity);

        btRigidBody *get();
        void activate();

        const Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> &transform();

        float mass() const;
        void setMass(float mass);

        bool kinematic() const;
        void setKinematic(bool kinematic);

        float friction() const;
        void setFriction(float friction);

        Vector3 linearFactor() const;
        void setLinearFactor(const Vector3 &factor);

        Vector3 angularFactor() const;
        void setAngularFactor(const Vector3 &factor);

        void setVelocity(const Vector3 &v);

        void setShape(typename CollisionShapeManager::HandleType handle);
        void setShapeName(const std::string_view &name);
        CollisionShapeManager::ResourceType *getShape() const;
        CollisionShapeInstance *getShapeInstance() const;

        friend struct PhysicsManager;

    private:
        typename CollisionShapeManager::InstanceHandle mShapeHandle;
        struct Data;
        std::unique_ptr<Data> mData;

        static Scene::SceneManager *sceneMgrFromData(Data *data);
    };

    using RigidBodyPtr = Scene::Entity::EntityComponentPtr<RigidBody>;

}
}

RegisterType(Engine::Physics::RigidBody);