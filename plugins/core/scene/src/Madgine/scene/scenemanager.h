#pragma once

#include "Meta/serialize/hierarchy/toplevelunit.h"

#include "scenecomponentcollector.h"

#include "Meta/serialize/container/syncablecontainer.h"

#include "Madgine/app/globalapibase.h"
#include "Madgine/app/globalapicollector.h"
#include "Meta/serialize/container/noparent.h"

#include "Modules/threading/datamutex.h"

#include "Modules/threading/signalfunctor.h"

#include "scenecomponentbase.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "entity/entityptr.h"

#include "entity/entitycomponentcollector.h"

#include "Generic/container/refcountedcontainer.h"
#include "entity/entity.h"

#include "Generic/keyvalue.h"

#include "Generic/container/concatIt.h"

#include "Meta/math/vector3.h"

#include "Generic/intervalclock.h"

namespace Engine {
namespace Scene {
    struct MADGINE_SCENE_EXPORT SceneManager : Serialize::TopLevelUnit<SceneManager>,
                                               App::GlobalAPI<Serialize::NoParent<SceneManager>> {
        SERIALIZABLEUNIT(SceneManager)

        using EntityContainer = RefcountedContainer<std::deque<Entity::Entity>>;

        SceneManager(App::Application &app);
        SceneManager(const SceneManager &) = delete;
        ~SceneManager();

        virtual std::string_view key() const override;

        void updateFrame();

        Entity::EntityPtr createEntity(const std::string &behavior = "", const std::string &name = "",
            const std::function<void(Entity::Entity &)> &init = {});
        void createEntityAsyncImpl(Serialize::GenericMessageReceiver receiver, const std::string &behavior = "", const std::string &name = "",
            const std::function<void(Entity::Entity &)> &init = {});
        ASYNC_STUB(createEntityAsync, createEntityAsyncImpl, Serialize::make_message_sender<Entity::EntityPtr>);

        Entity::EntityPtr createLocalEntity(const std::string &behavior = "", const std::string &name = "");
        Entity::EntityPtr findEntity(const std::string &name);
        void remove(Entity::Entity *e);

        //Entity::Entity *makeLocalCopy(Entity::Entity &e);
        Entity::Entity *makeLocalCopy(Entity::Entity &&e);

        void clear();

        void pause();
        bool unpause();
        bool isPaused() const;
        const Threading::CustomClock &clock() const;

        template <typename T>
        T &getComponent()
        {
            return static_cast<T &>(getComponent(UniqueComponent::component_index<T>()));
        }
        SceneComponentBase &getComponent(size_t i);
        size_t getComponentCount();

        Threading::DataMutex::Moded mutex(AccessMode mode);

        Threading::SignalStub<const EntityContainer::iterator &, int> &entitiesSignal();

        template <typename T>
        Entity::EntityComponentList<T> &entityComponentList()
        {
            return static_cast<Entity::EntityComponentList<T> &>(*mEntityComponentLists.at(UniqueComponent::component_index<T>()));
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
        virtual Threading::Task<bool> init() final;
        virtual Threading::Task<void> finalize() final;

    private:
        struct Clock : Threading::CustomClock {
            virtual std::chrono::steady_clock::time_point get(std::chrono::steady_clock::time_point timepoint) const override;
            virtual std::chrono::steady_clock::time_point revert(std::chrono::steady_clock::time_point timepoint) const override;

            std::chrono::steady_clock::duration mPauseAcc = std::chrono::steady_clock::duration::zero();
            std::chrono::steady_clock::time_point mPauseStart;
            std::atomic<size_t> mPauseStack = 0;
        } mClock;

        Threading::DataMutex mMutex;

        IntervalClock<Threading::CustomTimepoint> mSceneClock;
        IntervalClock<std::chrono::steady_clock::time_point> mFrameClock;

        ////////////////////////////////////////////// ECS

    private:
        std::string generateUniqueName();

        Serialize::StreamResult readNonLocalEntity(Serialize::FormattedSerializeStream &in, OutRef<SceneManager> &mgr, bool &isLocal, std::string &name);
        std::tuple<SceneManager &, bool, std::string> createEntityData(const std::string &name, bool local);
        const char *writeEntity(Serialize::FormattedSerializeStream &out, const Entity::Entity &entity) const;

    private:
        Entity::EntityComponentListContainer<std::vector<Placeholder<0>>> mEntityComponentLists;

        SYNCABLE_CONTAINER(mEntities, EntityContainer, Threading::SignalFunctor<const EntityContainer::iterator &, int>);
        container_api<RefcountedContainer<std::deque<Serialize::NoParent<Entity::Entity>>>> mLocalEntities;

        struct EntityHelper {
            Entity::EntityPtr operator()(Entity::Entity &ref)
            {
                return { &ref };
            }
        };

    public:
        using ControlBlock = typename EntityContainer::ControlBlock;

        decltype(auto) entities()
        {
            return concatIt(mEntities, mLocalEntities) | std::views::transform(EntityHelper {});
        }

        ////////////////////////////////////////// ECS End

    public:
        MEMBER_OFFSET_CONTAINER(mSceneComponents, , SceneComponentContainer<Serialize::SerializableContainer<std::set<Placeholder<0>, KeyCompare<Placeholder<0>>>, NoOpFunctor>>);
    };

}
}

REGISTER_TYPE(Engine::Scene::SceneManager)
REGISTER_TYPE(Engine::Serialize::NoParent<Engine::Scene::SceneManager>)