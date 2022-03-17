#pragma once

#include "Meta/keyvalue/objectptr.h"

#include "Meta/serialize/container/serializablecontainer.h"

#include "Meta/serialize/hierarchy/syncableunit.h"

#include "Generic/container/transformIt.h"
#include "Generic/container/mutable_set.h"

#include "Generic/functor.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Entity : Serialize::SyncableUnit<Entity> {
            SERIALIZABLEUNIT(Entity);

            //Entity(const Entity &, bool local);
            Entity(Entity &&, bool local);
            Entity(Entity &&);

            Entity(SceneManager &sceneMgr, bool local, const std::string &name, const ObjectPtr &behavior = {});
            Entity(const Entity &) = delete;
            ~Entity();

            Entity &operator=(Entity &&other);

            void remove();

            const std::string &key() const;

            const std::string &name() const;

            template <typename T>
            EntityComponentPtr<T> addComponent(const ObjectPtr &table = {})
            {
                return EntityComponentPtr<T> { addComponent(component_index<T>(), table) };
            }

            template <typename T>
            void removeComponent()
            {
                removeComponent(component_index<T>());
            }

            template <typename T>
            EntityComponentPtr<T> getComponent()
            {
                return static_cast<EntityComponentPtr<T>>(getComponent(component_index<T>()));
            }

            template <typename T>
            EntityComponentPtr<const T> getComponent() const
            {
                return static_cast<EntityComponentPtr<const T>>(getComponent(component_index<T>()));
            }

            EntityComponentPtr<EntityComponentBase> getComponent(uint32_t i);
            EntityComponentPtr<const EntityComponentBase> getComponent(uint32_t i) const;
            EntityComponentPtr<EntityComponentBase> getComponent(std::string_view name);
            EntityComponentPtr<const EntityComponentBase> getComponent(std::string_view name) const;

            struct MADGINE_SCENE_EXPORT Helper {
                Entity *mEntity;
                EntityComponentPtr<EntityComponentBase> operator()(const EntityComponentOwningHandle<EntityComponentBase> &p);
            };
            decltype(auto) components()
            {                
                return transformIt(mComponents, Helper { this });
            }

            template <typename T>
            bool hasComponent()
            {
                return hasComponent(component_index<T>());
            }

            bool hasComponent(size_t i);
            bool hasComponent(std::string_view name);

            EntityComponentPtr<EntityComponentBase> addComponent(std::string_view name, const ObjectPtr &table = {});
            EntityComponentPtr<EntityComponentBase> addComponent(size_t i, const ObjectPtr &table = {});
            void removeComponent(std::string_view name);
            void removeComponent(size_t i);
            void clearComponents();
            void relocateComponent(EntityComponentHandle<EntityComponentBase> newIndex);

            bool isLocal() const;

            void handleEntityEvent(const typename std::set<EntityComponentOwningHandle<EntityComponentBase>>::iterator &it, int op);

            SceneManager &sceneMgr();
            const SceneManager &sceneMgr() const;

        public:
            std::string mName;

        private:
            Serialize::StreamResult readComponent(Serialize::FormattedSerializeStream &in, EntityComponentOwningHandle<EntityComponentBase> &handle);
            const char *writeComponent(Serialize::FormattedSerializeStream &out, const EntityComponentOwningHandle<EntityComponentBase> &comp) const;

            bool mLocal;

            SERIALIZABLE_CONTAINER(mComponents, mutable_set<EntityComponentOwningHandle<EntityComponentBase>, std::less<>>, ParentFunctor<&Entity::handleEntityEvent>);

            SceneManager &mSceneManager;
        };

    }
}
}
