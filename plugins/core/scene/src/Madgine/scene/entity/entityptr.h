#pragma once

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT EntityPtr : ScopeBase {

            EntityPtr() = default;
            ~EntityPtr();

            EntityPtr(const EntityPtr &other);
            EntityPtr(ControlBlock<Entity> &entity);

            EntityPtr &operator=(const EntityPtr &other);
            EntityPtr &operator=(EntityPtr &&other);

            Entity *operator->() const;

            void update() const;
            void reset();
            //operator bool() const;

            operator Entity *() const;
            Entity *get() const;

            bool operator==(const EntityPtr &other) const;
            bool operator<(const EntityPtr &other) const;

            bool isDead() const;

            void readState(Serialize::SerializeInStream &in, const char *name = nullptr);
            void writeState(Serialize::SerializeOutStream &out, const char *name = nullptr) const;

            void applySerializableMap(Serialize::SerializeInStream &in);

        private:
            bool holdsRef() const;
            ControlBlock<Entity> *getBlock() const;

        private:
            mutable uintptr_t mEntity = 0;
        };

    }
}
}