#pragma once

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT EntityPtr {

            EntityPtr() = default;
            ~EntityPtr();

            EntityPtr(const EntityPtr &other);
            EntityPtr(Entity *entity);

            EntityPtr &operator=(const EntityPtr &other);
            EntityPtr &operator=(EntityPtr &&other);

            Entity *operator->() const;

            void update() const;
            void reset();
            //operator bool() const;

            operator Entity *() const;
            Entity *get() const;

            bool operator==(const EntityPtr &other) const;
            bool operator==(Entity *other) const;
            std::strong_ordering operator<=>(const EntityPtr &other) const;

            bool isDead() const;

            Serialize::StreamResult readState(Serialize::SerializeInStream &in, const char *name = nullptr);
            void writeState(Serialize::SerializeOutStream &out, const char *name = nullptr) const;

            void applySerializableMap(Serialize::SerializeInStream &in, bool success);

        private:
            bool holdsRef() const;
            struct ControlBlockDummy;
            ControlBlockDummy getBlock() const;

        private:
            mutable uintptr_t mEntity = 0;
        };

    }
}
}