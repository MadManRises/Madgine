#pragma once

#include "Modules/serialize/toplevelserializableunit.h"

#include "scenecomponentcollector.h"

#include "Modules/serialize/container/syncablecontainer.h"

#include "Madgine/app/globalapicollector.h"
#include "Modules/madgineobject/madgineobjectobserver.h"
#include "Modules/serialize/container/noparent.h"

#include "Modules/serialize/container/controlledcontainer.h"

#include "Modules/threading/datamutex.h"

#include "Modules/threading/signalfunctor.h"

#include "Modules/keyvalue/keyvalueset.h"

namespace Engine {
namespace Scene {
    struct MADGINE_SCENE_EXPORT SceneManager : Serialize::TopLevelSerializableUnit<SceneManager>,
                                               App::GlobalAPI<Serialize::NoParentUnit<SceneManager>> {
        SERIALIZABLEUNIT;

        SceneManager(App::Application &app);
        SceneManager(const SceneManager &) = delete;
        ~SceneManager();

        void update();

        Entity::Entity *createEntity(const std::string &behavior = "", const std::string &name = "",
            const std::function<void(Entity::Entity &)> &init = {});
        Entity::Entity *createLocalEntity(const std::string &behavior = "", const std::string &name = "");
        Entity::Entity *findEntity(const std::string &name);
        void removeLater(Entity::Entity *e);

        Entity::Entity *makeLocalCopy(Entity::Entity &e);
        Entity::Entity *makeLocalCopy(Entity::Entity &&e);

        void clear();

        template <typename T>
        T &getComponent(bool init = true)
        {
            return static_cast<T &>(getComponent(T::component_index(), init));
        }
        SceneComponentBase &getComponent(size_t i, bool = true);
        size_t getComponentCount();

        template <typename T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(T::component_index(), init));
        }

        App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

        SceneManager &getSelf(bool = true);

        virtual const MadgineObject *parent() const override;

        Threading::DataMutex &mutex();

        void removeQueuedEntities();

        Threading::SignalStub<const std::list<Engine::Scene::Entity::Entity>::iterator &, int> &entitiesSignal();

    protected:
        virtual bool init() final;
        virtual void finalize() final;

    private:
        std::string generateUniqueName();

        std::tuple<SceneManager &, bool, std::string> createNonLocalEntityData(const std::string &name);
        std::tuple<SceneManager &, bool, std::string> createEntityData(const std::string &name, bool local);

    private:
        App::Application &mApp;
        size_t mItemCount;

    public:
        OFFSET_CONTAINER(mSceneComponents, SceneComponentContainer<Serialize::ControlledContainer<KeyValueSet<Placeholder<0>>, MadgineObjectObserver>>);

    private:
        SYNCABLE_CONTAINER(mEntities, std::list<Entity::Entity>, Serialize::ContainerPolicies::masterOnly, Threading::SignalFunctor<const std::list<Engine::Scene::Entity::Entity>::iterator &, int>);
        std::list<Entity::Entity> mLocalEntities;
        std::list<Entity::Entity *> mEntityRemoveQueue;

        Threading::DataMutex mMutex;

        std::chrono::steady_clock::time_point mLastFrame;

    public:
        decltype(mEntities) &entities();

        //Threading::SignalStub<const decltype(mEntities)::iterator &, int> &entitiesSignal();
    };

}
}
