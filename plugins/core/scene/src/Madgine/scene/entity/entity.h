#pragma once

#include "Modules/keyvalue/scopebase.h"

#include "entitycomponentbase.h"

#include "Modules/serialize/container/syncablecontainer.h"

#include "Modules/keyvalue/keyvalueset.h"

#include "Modules/generic/future.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Entity : Serialize::SerializableUnit<Entity>, ScopeBase {
            SERIALIZABLEUNIT;

            //Entity(const Entity &, bool local);
            Entity(Entity &&, bool local);
            Entity(Entity &&);

            Entity(SceneManager &sceneMgr, bool local, const std::string &name, const ObjectPtr &behavior = {});
            Entity(const Entity &) = delete;
            ~Entity();

            void remove();

            const std::string &key() const;

            const std::string &name() const;

            template <typename T>
            auto addComponent(const ObjectPtr &table = {})
            {
                return addComponent(component_index<T>(), table).then([](EntityComponentBase *comp) { return static_cast<T *>(comp); });
            }

            template <typename T>
            void removeComponent()
            {
                removeComponent(component_index<T>());
            }

            template <typename T>
            T *getComponent()
            {
                return static_cast<T *>(getComponent(component_index<T>()));
            }

            template <typename T>
            const T *getComponent() const
            {
                return static_cast<const T *>(getComponent(component_index<T>()));
            }

            EntityComponentBase *getComponent(size_t i);
            const EntityComponentBase *getComponent(size_t i) const;
            EntityComponentBase *getComponent(const std::string_view &name);
            const EntityComponentBase *getComponent(const std::string_view &name) const;

            static EntityComponentBase *toEntityComponentPtr(const std::pair<const size_t, std::unique_ptr<EntityComponentBase>> &p);

            decltype(auto) components()
            {
                return transformIt<Functor<&toEntityComponentPtr>>(mComponents);
            }

            template <typename T>
            bool hasComponent()
            {
                return hasComponent(component_index<T>());
            }

            bool hasComponent(size_t i);
            bool hasComponent(const std::string_view &name);

            Future<EntityComponentBase *> addComponent(const std::string_view &name, const ObjectPtr &table = {});
            Future<EntityComponentBase *> addComponent(size_t i, const ObjectPtr &table = {});
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

            void swap(Entity &other);

            void handleEntityEvent(const typename std::map<size_t, std::unique_ptr<EntityComponentBase>>::iterator &it, int op);

        protected:
            EntityComponentBase *addComponentSimple(const std::string_view &name, const ObjectPtr &table = {});

        public:
            std::string mName;

        private:
            std::tuple<size_t, std::unique_ptr<EntityComponentBase>> createComponentTuple(const std::string &name);
            std::tuple<std::pair<const char *, std::string_view>> storeComponentCreationData(const std::pair<const size_t, std::unique_ptr<EntityComponentBase>> &comp) const;

            bool mLocal;

            SYNCABLE_CONTAINER(mComponents, std::map<size_t, std::unique_ptr<EntityComponentBase>>, Serialize::ContainerPolicies::masterOnly, ParentFunctor<&Entity::handleEntityEvent>);

            SceneManager &mSceneManager;
        };

        inline void swap(Entity& first, Entity& second) {
            first.swap(second);
        }

    }
}
}

RegisterType(Engine::Scene::Entity::Entity);