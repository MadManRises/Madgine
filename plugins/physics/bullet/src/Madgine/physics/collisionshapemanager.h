#pragma once

#include "Modules/resources/resourceloader.h"

class btCollisionShape;

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