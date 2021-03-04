#pragma once

#include "Meta/serialize/toplevelunit.h"

#include "scenecomponentcollector.h"

#include "Meta/serialize/container/syncablecontainer.h"

#include "Madgine/app/globalapibase.h"
#include "Madgine/app/globalapicollector.h"
#include "madgineobject/madgineobjectobserver.h"
#include "Meta/serialize/container/noparent.h"

#include "Modules/threading/datamutex.h"

#include "Modules/threading/signalfunctor.h"

#include "scenecomponentbase.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "entity/entityptr.h"

#include "entity/entitycomponentcollector.h"

#include "Generic/container/refcounted_deque.h"
#include "entity/entity.h"

#include "Generic/keyvalue.h"

namespace Engine {
namespace Scene {
    struct MADGINE_SCENE_EXPORT SceneManager : Serialize::TopLevelUnit<SceneManager>,
                                               App::GlobalAPI<Serialize::NoParentUnit<SceneManager>> {
        SERIALIZABLEUNIT(SceneManager);

        SceneManager(App::Application &app);
        SceneManager(const SceneManager &) = delete;
        ~SceneManager();

        void update();

        Future<Entity::EntityPtr> createEntity(const std::string &behavior = "", const std::string &name = "",
            const std::function<void(Entity::Entity&)> &init = {});
        Entity::EntityPtr createLocalEntity(const std::string &behavior = "", const std::string &name = "");
        Entity::EntityPtr findEntity(const std::string &name);
        void remove(Entity::Entity *e);

        //Entity::Entity *makeLocalCopy(Entity::Entity &e);
        Entity::Entity *makeLocalCopy(Entity::Entity &&e);

        void clear();

        void pause();
        bool unpause();
        bool isPaused() const;

        template <typename T>
        T &getComponent(bool init = true)
        {
            return static_cast<T &>(getComponent(Engine::component_index<T>(), init));
        }
        SceneComponentBase &getComponent(size_t i, bool = true);
        size_t getComponentCount();

        template <typename T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(Engine::component_index<T>(), init));
        }

        App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

        Threading::DataMutex &mutex();

        Threading::SignalStub<const refcounted_deque<Entity::Entity>::iterator &, int> &entitiesSignal();

        template <typename T>
        Entity::EntityComponentList<T> &entityComponentList()
        {
            return static_cast<Entity::EntityComponentList<T> &>(*mEntityComponentLists.at(Engine::component_index<T>()));
        }

        Entity::EntityComponentListBase &entityComponentList(size_t index)
        {
            return *mEntityComponentLists.at(index);
        }

        const Entity::EntityComponentListBase &entityComponentList(size_t index) const
        {
            return *mEntityComponentLists.at(index);
        }

    protected:
        virtual bool init() final;
        virtual void finalize() final;

    private:
        std::string generateUniqueName();

        std::tuple<SceneManager &, bool, std::string> createNonLocalEntityData(const std::string &name);
        std::tuple<SceneManager &, bool, std::string> createEntityData(const std::string &name, bool local);
        std::tuple<std::pair<const char *, std::string>> storeEntityCreationData(const Entity::Entity &entity) const;

    public:
        OFFSET_CONTAINER(mSceneComponents, SceneComponentContainer<Serialize::SerializableContainer<std::set<Placeholder<0>, KeyCompare<Placeholder<0>>>, MadgineObjectObserver, std::true_type>>);

    private:
        Entity::EntityComponentListContainer<std::vector<Placeholder<0>>> mEntityComponentLists;

        SYNCABLE_CONTAINER(mEntities, refcounted_deque<Entity::Entity>, Threading::SignalFunctor<const refcounted_deque<Entity::Entity>::iterator &, int>);
        refcounted_deque<Entity::Entity> mLocalEntities;

        Threading::DataMutex mMutex;

        std::chrono::steady_clock::time_point mLastFrame;

        size_t mItemCount = 0;

        std::atomic<size_t> mPauseStack = 0;

    public:
        decltype(auto) entities()
        {
            struct Helper {
                Entity::EntityPtr operator()(ControlBlock<Entity::Entity> &ref)
                {
                    return { ref };
                }
            };
            return transformIt<Helper>(mEntities.blocks());
        }
    };

}
}

RegisterType(Engine::Scene::SceneManager);
RegisterType(Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>);