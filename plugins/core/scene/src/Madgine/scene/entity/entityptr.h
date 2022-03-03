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

        private:
            bool holdsRef() const;
            struct ControlBlockDummy;
            ControlBlockDummy getBlock() const;

            friend struct Serialize::Operations<EntityPtr>;

        private:
            mutable uintptr_t mEntity = 0;
        };

    }
}
namespace Serialize {

    template <>
    struct MADGINE_SCENE_EXPORT Operations<Scene::Entity::EntityPtr> {
        static StreamResult read(FormattedSerializeStream &in, Scene::Entity::EntityPtr &ptr, const char *name = nullptr);
        static void write(Serialize::FormattedSerializeStream &out, const Scene::Entity::EntityPtr &ptr, const char *name = nullptr);

        static StreamResult applyMap(FormattedSerializeStream &in, Scene::Entity::EntityPtr &ptr, bool success);
    };

}
}