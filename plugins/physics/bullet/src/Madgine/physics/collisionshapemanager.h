#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Meta/serialize/virtualserializableunit.h"

class btCollisionShape;

namespace Engine {
namespace Physics {

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
            InstanceHandle(const InstanceHandle &);
            InstanceHandle(InstanceHandle &&other) noexcept;
            ~InstanceHandle();

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
            CollisionShapeInstance *mInstance = nullptr;
        };

        CollisionShapeManager();

        bool loadImpl(std::unique_ptr<CollisionShape> &shape, ResourceDataInfo &info);
        void unloadImpl(std::unique_ptr<CollisionShape> &shape, ResourceDataInfo &info);
    };

    
    struct CollisionShapeInstance : Serialize::VirtualSerializableUnitBase<VirtualScopeBase<>, Serialize::SerializableUnitBase> {
        virtual ~CollisionShapeInstance() = default;
        virtual btCollisionShape *get() = 0;
        virtual bool isInstance() = 0;
        virtual CollisionShapeInstance *clone() = 0;

        typename CollisionShapeManager::HandleType mShape;
    };

}
}

RegisterType(Engine::Physics::CollisionShapeManager);