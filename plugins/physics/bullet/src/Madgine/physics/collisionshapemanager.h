#pragma once

#include "Modules/resources/resourceloader.h"

class btCollisionShape;

namespace Engine {
namespace Physics {

    struct CollisionShapeManager;
    struct CollisionShapeInstance;

    struct CollisionShape {
        virtual ~CollisionShape() = default;
        virtual CollisionShapeInstance *create() = 0;
    };


    struct CollisionShapeManager : Resources::ResourceLoader<CollisionShapeManager, std::unique_ptr<CollisionShape>> {

        struct InstanceHandle {

            InstanceHandle() = default;
            InstanceHandle(HandleType shape);
            InstanceHandle(ResourceType *res);
            InstanceHandle(const InstanceHandle &) = delete;
            InstanceHandle(InstanceHandle &&other);
            ~InstanceHandle();

            InstanceHandle &operator=(InstanceHandle &&other);

            void load(const std::string_view &name, CollisionShapeManager *loader = nullptr);

            ResourceType *resource() const;

            CollisionShapeInstance *operator->() const;

            operator CollisionShapeInstance *() const;

        private:
            CollisionShapeInstance *mInstance = nullptr;
        };

        CollisionShapeManager();

        bool loadImpl(std::unique_ptr<CollisionShape> &shape, ResourceType *res);
        void unloadImpl(std::unique_ptr<CollisionShape> &shape, ResourceType *res);
    };

    
    struct CollisionShapeInstance : VirtualScopeBase {
        virtual ~CollisionShapeInstance() = default;
        virtual btCollisionShape *get() = 0;
        virtual bool isInstance() = 0;

        typename CollisionShapeManager::HandleType mShape;
    };

}
}

RegisterType(Engine::Physics::CollisionShapeManager);