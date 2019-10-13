#pragma once

#include "Modules/serialize/toplevelserializableunit.h"

#include "scenecomponentcollector.h"

#include "Modules/serialize/container/syncablecontainer.h"

#include "entity/entity.h"

#include "../app/globalapicollector.h"
#include "Modules/keyvalue/observablecontainer.h"
#include "Modules/madgineobject/madgineobjectobserver.h"
#include "Modules/serialize/container/noparent.h"

#include "Modules/serialize/container/controlledcontainer.h"

#include "Modules/threading/datamutex.h"

namespace Engine {
namespace Scene {
    class MADGINE_BASE_EXPORT SceneManager : public Serialize::TopLevelSerializableUnit<SceneManager>,
                                             public App::GlobalAPI<Serialize::NoParentUnit<SceneManager>>{
        SERIALIZABLEUNIT;

    public:
        SceneManager(App::Application &app);
        SceneManager(const SceneManager &) = delete;
        virtual ~SceneManager() = default;

        /*bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;
        bool frameFixedUpdate(std::chrono::microseconds timeStep, ContextMask context) override final;*/

        Entity::Entity *createEntity(const std::string &behavior = "", const std::string &name = "",
            const std::function<void(Entity::Entity &)> &init = {});
        Entity::Entity *createLocalEntity(const std::string &behavior = "", const std::string &name = "");
        Entity::Entity *findEntity(const std::string &name);
        void removeLater(Entity::Entity *e);

        Entity::Entity *makeLocalCopy(Entity::Entity &e);
        Entity::Entity *makeLocalCopy(Entity::Entity &&e);

        void clear();

        template <class T>
        T &getComponent(bool init = true)
        {
            return static_cast<T &>(getComponent(T::component_index(), init));
        }
        SceneComponentBase &getComponent(size_t i, bool = true);
        size_t getComponentCount();

        template <class T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(T::component_index(), init));
        }

        App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

        SceneManager &getSelf(bool = true);

        virtual const MadgineObject *parent() const override;

        Threading::DataMutex &mutex();

        SignalSlot::SignalStub<> &clearedSignal();

		void removeQueuedEntities();

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
        SERIALIZABLE_CONTAINER_EXT(mSceneComponents, SceneComponentContainer<PartialObservableContainer<elevate<, Serialize::ControlledContainer, ,std::set>::type, MadgineObjectObserver>::type>);

    private:
        SYNCABLE_CONTAINER(mEntities, std::list<Entity::Entity>, Serialize::ContainerPolicies::masterOnly);
        std::list<Serialize::NoParentUnit<Entity::Entity>> mLocalEntities;
        std::list<Entity::Entity *> mEntityRemoveQueue;

        SignalSlot::Signal<> mStateLoadedSignal;
        SignalSlot::Signal<> mClearedSignal;

        Threading::DataMutex mMutex;

    public:
        decltype(mEntities) &entities();

        //SignalSlot::SignalStub<const decltype(mEntities)::iterator &, int> &entitiesSignal();
    };

}
}
