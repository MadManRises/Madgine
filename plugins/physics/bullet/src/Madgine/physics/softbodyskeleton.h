#pragma once

#include "Madgine/scene/entity/entitycomponent.h"

#include "Madgine/scene/entity/entitycomponentptr.h"

#include "Madgine/scene/entity/components/transform.h"

#include "Madgine/scene/entity/components/skeleton.h"

class btSoftBody;

namespace Engine {
namespace Physics {

    struct MADGINE_BULLET_EXPORT SoftBodySkeleton : Engine::Scene::Entity::EntityComponent<SoftBodySkeleton> {
        SERIALIZABLEUNIT(SoftBodySkeleton);

        SoftBodySkeleton(const ObjectPtr &data = {});
        SoftBodySkeleton(SoftBodySkeleton &&other);
        ~SoftBodySkeleton();

        SoftBodySkeleton &operator=(SoftBodySkeleton &&other);

        void init(Scene::Entity::Entity *entity);
        void finalize(Scene::Entity::Entity *entity);

        btSoftBody *get();
        void activate();

        const Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> &transform();


        void attach(RigidBody *rigidbody, size_t index, const Engine::Vector3 &offset = Engine::Vector3::ZERO);
        /* float mass() const;
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
        void setShapeName(std::string_view name);
        CollisionShapeManager::ResourceType *getShape() const;
        CollisionShapeInstance *getShapeInstance() const;
        */
        friend struct PhysicsManager;

    private:
        //typename CollisionShapeManager::InstanceHandle mShapeHandle;
        struct Data;
        std::unique_ptr<Data> mData;

        //static Scene::SceneManager *sceneMgrFromData(Data *data);
    };

    using SoftBodySkeletonPtr = Scene::Entity::EntityComponentPtr<SoftBodySkeleton>;

}
}

RegisterType(Engine::Physics::SoftBodySkeleton);
RegisterType(Engine::Scene::Entity::EntityComponentList<Engine::Physics::SoftBodySkeleton>);