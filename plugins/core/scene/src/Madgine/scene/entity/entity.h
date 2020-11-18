#pragma once

#include "Modules/keyvalue/scopebase.h"

#include "entitycomponentbase.h"

#include "Modules/serialize/container/syncablecontainer.h"

#include "Modules/keyvalue/keyvalueset.h"

#include "Modules/generic/future.h"

#include "entitycomponentptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Entity : Serialize::SyncableUnit<Entity>, ScopeBase {
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
            auto addComponent(const EntityPtr &self, const ObjectPtr &table = {})
            {
                return addComponent(component_index<T>(), self, table).then([](const EntityComponentPtr<EntityComponentBase> &comp) { return static_cast<EntityComponentPtr<T>>(comp); });
            }

            template <typename T>
            void removeComponent()
            {
                removeComponent(component_index<T>());
            }

            template <typename T>
            EntityComponentPtr<T> getComponent(const EntityPtr &self)
            {
                return static_cast<EntityComponentPtr<T>>(getComponent(component_index<T>(), self));
            }

            template <typename T>
            EntityComponentPtr<const T> getComponent(const EntityPtr &self) const
            {
                return static_cast<EntityComponentPtr<const T>>(getComponent(component_index<T>(), self));
            }

            EntityComponentPtr<EntityComponentBase> getComponent(size_t i, const EntityPtr &self);
            EntityComponentPtr<const EntityComponentBase> getComponent(size_t i, const EntityPtr &self) const;
            EntityComponentPtr<EntityComponentBase> getComponent(const std::string_view &name, const EntityPtr &self);
            EntityComponentPtr<const EntityComponentBase> getComponent(const std::string_view &name, const EntityPtr &self) const;

            EntityComponentBase *toEntityComponentPtr(const std::pair<const uint32_t, EntityComponentOwningHandle<EntityComponentBase>> &p);

            decltype(auto) components()
            {
                struct Helper {
                    Entity *mEntity;
                    EntityComponentBase *operator()(const std::pair<const uint32_t, EntityComponentOwningHandle<EntityComponentBase>> &p)
                    {
                        return mEntity->toEntityComponentPtr(p);
                    }
                };
                return transformIt(mComponents, Helper { this });
            }

            template <typename T>
            bool hasComponent()
            {
                return hasComponent(component_index<T>());
            }

            bool hasComponent(size_t i);
            bool hasComponent(const std::string_view &name);

            EntityComponentPtr<EntityComponentBase> addComponent(const std::string_view &name, const EntityPtr &self, const ObjectPtr &table = {});
            EntityComponentPtr<EntityComponentBase> addComponent(size_t i, const EntityPtr &self, const ObjectPtr &table = {});
            void removeComponent(const std::string_view &name);
            void removeComponent(size_t i);
            void clearComponents();

            SceneManager &sceneMgr(bool = true) const;

            bool isLocal() const;

            template <typename T>
            T &getSceneComponent(bool init = true)
            {
                return static_cast<T &>(getSceneComponent(component_index<T>(), init));
            }

            SceneComponentBase &getSceneComponent(size_t i, bool = true);

            template <typename T>
            T &getGlobalAPIComponent(bool init = true)
            {
                return static_cast<T &>(getGlobalAPIComponent(component_index<T>(), init));
            }

            App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

            void handleEntityEvent(const typename std::map<uint32_t, EntityComponentOwningHandle<EntityComponentBase>>::iterator &it, int op);

            void update();

        public:
            std::string mName;

        private:
            std::tuple<uint32_t, EntityComponentOwningHandle<EntityComponentBase>> createComponentTuple(const std::string &name);
            std::tuple<std::pair<const char *, std::string_view>> storeComponentCreationData(const std::pair<const uint32_t, EntityComponentOwningHandle<EntityComponentBase>> &comp) const;

            bool mLocal;

            SERIALIZABLE_CONTAINER(mComponents, std::map<uint32_t, EntityComponentOwningHandle<EntityComponentBase>>, ParentFunctor<&Entity::handleEntityEvent>);

            SceneManager &mSceneManager;
        };

    }
}
}

RegisterType(Engine::Scene::Entity::Entity);