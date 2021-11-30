#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Meta/serialize/virtualserializableunit.h"

class btCollisionShape;

namespace Engine {
namespace Physics {

    struct CollisionShapeInstance;

    struct CollisionShape;

    struct CollisionShapeInstanceDeleter {
        void operator()(CollisionShapeInstance *);
    };

    using CollisionShapeInstancePtr = std::unique_ptr<CollisionShapeInstance, CollisionShapeInstanceDeleter>;

    struct CollisionShapeManager : Resources::ResourceLoader<CollisionShapeManager, std::unique_ptr<CollisionShape>> {

        struct InstanceHandle {

            InstanceHandle() = default;
            InstanceHandle(HandleType shape);
            InstanceHandle(ResourceType *res);
            InstanceHandle(const InstanceHandle &);
            InstanceHandle(InstanceHandle &&other) noexcept = default;

            InstanceHandle &operator=(const InstanceHandle &other);
            InstanceHandle &operator=(InstanceHandle &&other);

            void load(std::string_view name, CollisionShapeManager *loader = nullptr);
            void reset();

            ResourceType *resource() const;

            CollisionShapeInstance *operator->() const;

            operator CollisionShapeInstance *() const;

            Serialize::StreamResult readState(Serialize::SerializeInStream &in, const char *name = nullptr);
            void writeState(Serialize::SerializeOutStream &out, const char *name = nullptr) const;

        private:
            CollisionShapeInstancePtr mInstance;
        };

        CollisionShapeManager();

        Threading::ImmediateTask<bool> loadImpl(std::unique_ptr<CollisionShape> &shape, ResourceDataInfo &info);
        void unloadImpl(std::unique_ptr<CollisionShape> &shape, ResourceDataInfo &info);        
    };

    struct CollisionShape {
        virtual ~CollisionShape() = default;
        virtual CollisionShapeInstancePtr create(typename CollisionShapeManager::HandleType shape) = 0;
    };

    struct CollisionShapeInstance : Serialize::VirtualSerializableUnitBase<VirtualScopeBase<>, Serialize::SerializableUnitBase> {
        CollisionShapeInstance(typename CollisionShapeManager::HandleType shape = {});        
        virtual ~CollisionShapeInstance();
        virtual btCollisionShape *get() = 0;
        virtual bool isInstance() = 0;
        virtual CollisionShapeInstancePtr clone() = 0;

        CollisionShapeManager::ResourceType *resource() const;

        bool available() const;

    protected:
        typename CollisionShapeManager::HandleType mHandle;
    };

}
}

RegisterType(Engine::Physics::CollisionShapeManager);