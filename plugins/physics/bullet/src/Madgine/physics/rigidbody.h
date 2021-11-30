#pragma once

#include "Madgine/scene/entity/entitycomponent.h"

#include "Madgine/scene/entity/entitycomponentptr.h"

#include "collisionshapemanager.h"

class btRigidBody;

namespace Engine {
namespace Physics {

    struct MADGINE_BULLET_EXPORT RigidBody : Scene::Entity::EntityComponent<RigidBody> {
        SERIALIZABLEUNIT(RigidBody);

        using Container = FreeListContainer<Scene::Entity::EntityComponentContainerImpl<std::vector>, Scene::Entity::EntityComponentFreeListConfig>;

        RigidBody(const ObjectPtr &data = {});
        RigidBody(RigidBody &&other);
        ~RigidBody();

        RigidBody &operator=(RigidBody &&other);

        void init(Scene::Entity::Entity *entity);
        void finalize(Scene::Entity::Entity *entity);

        void update();

        btRigidBody *get();
        const btRigidBody *get() const;
        void activate();

        Scene::Entity::Transform *transform();

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

        Vector3 angularVelocity() const;
        void setAngularVelocity(const Vector3 &v);

        void setVelocity(const Vector3 &v);

        Scene::SceneManager *sceneMgr();

        void setShape(typename CollisionShapeManager::HandleType handle);
        void setShapeName(std::string_view name);
        CollisionShapeManager::ResourceType *getShape() const;
        CollisionShapeInstance *getShapeInstance() const;

        void add();
        void remove();

        friend struct PhysicsManager;

    protected:
        typename CollisionShapeManager::InstanceHandle mShapeHandle;
        Scene::SceneManager *mSceneMgr;
        struct Data;
        std::unique_ptr<Data> mData;
    };

    using RigidBodyPtr = Scene::Entity::EntityComponentPtr<RigidBody>;

}
}

RegisterType(Engine::Physics::RigidBody);
RegisterType(Engine::Scene::Entity::EntityComponentList<Engine::Physics::RigidBody>);