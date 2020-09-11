#pragma once

#include "Modules/resources/resourceloader.h"

#include "bullet3-2.89/src/btBulletDynamicsCommon.h"


namespace Engine {
namespace Physics {

    struct CollisionShape {
        virtual ~CollisionShape() = default;
        virtual btCollisionShape *get() = 0;
    };

    struct CollisionShapeManager : Resources::ResourceLoader<CollisionShapeManager, std::unique_ptr<CollisionShape>> {

        CollisionShapeManager();

		bool loadImpl(std::unique_ptr<CollisionShape> &shape, ResourceType *res);
        void unloadImpl(std::unique_ptr<CollisionShape> &shape, ResourceType *res);

    };

}
}

RegisterType(Engine::Physics::CollisionShapeManager);