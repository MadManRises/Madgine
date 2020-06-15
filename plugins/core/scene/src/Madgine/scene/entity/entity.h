#pragma once

#include "Modules/keyvalue/scopebase.h"

#include "entitycomponentbase.h"

#include "Modules/serialize/container/syncablecontainer.h"

#include "Modules/keyvalue/keyvalueset.h"

#include "Modules/generic/future.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct EntityComponentObserver {
            void operator()(const typename KeyValueSet<std::unique_ptr<EntityComponentBase>>::iterator &it, int op);
        };

        struct MADGINE_SCENE_EXPORT Entity : Serialize::SerializableUnit<Entity>, ScopeBase {
            SERIALIZABLEUNIT;

            Entity(const Entity &, bool local);
            Entity(Entity &&, bool local);

            Entity(SceneManager &sceneMgr, bool local, const std::string &name, const ObjectPtr &behavior = {});
            Entity(const Entity &) = delete;
            ~Entity();

            void remove();

            const std::string &key() const;

            const std::string &name() const;

            template <typename T>
            Future<T *> addComponent(const ObjectPtr &table = {})
            {
                return addComponent(component_index<T>(), T::componentName(), table).then([](EntityComponentBase *comp) { return static_cast<T *>(comp); });
            }

            template <typename T>
            void removeComponent()
            {
                removeComponent(T::componentName());
            }

            template <typename T>
            T *getComponent()
            {
                return static_cast<T *>(getComponent(T::componentName()));
            }

            EntityComponentBase *getComponent(const std::string_view &name);

            decltype(auto) components()
            {
                return uniquePtrToPtr(mComponents);
            }

            template <typename T>
            bool hasComponent()
            {
                return hasComponent(T::componentName());
            }

            bool hasComponent(const std::string_view &name);

            Future<EntityComponentBase *> addComponent(const std::string_view &name, const ObjectPtr &table = {});
            Future<EntityComponentBase *> addComponent(size_t i, const std::string_view &name, const ObjectPtr &table = {});
            void removeComponent(const std::string_view &name);

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

        protected:
            EntityComponentBase *addComponentSimple(const std::string_view &name, const ObjectPtr &table = {});

        public:
            std::string mName;

        private:
            std::tuple<std::unique_ptr<EntityComponentBase>> createComponentTuple(const std::string &name);
            std::tuple<std::pair<const char *, std::string_view>> storeComponentCreationData(const std::unique_ptr<EntityComponentBase> &comp) const;

            bool mLocal;

            SYNCABLE_CONTAINER(mComponents, KeyValueSet<std::unique_ptr<EntityComponentBase>>, Serialize::ContainerPolicies::masterOnly, EntityComponentObserver);

            SceneManager &mSceneManager;
        };

    }
}
}

RegisterType(Engine::Scene::Entity::Entity);