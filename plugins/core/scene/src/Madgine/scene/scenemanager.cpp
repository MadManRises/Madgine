#include "../scenelib.h"

#include "scenemanager.h"

#include "Meta/serialize/hierarchy/serializableids.h"

#include "scenecomponentbase.h"

#include "Madgine/app/application.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "entity/entity.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "entity/entitycomponentlistbase.h"

#include "Meta/serialize/configs/controlled.h"

#include "Meta/serialize/configs/guard.h"

#include "Modules/threading/awaitables/awaitablesender.h"
#include "Modules/threading/awaitables/awaitabletimepoint.h"

#include "Generic/execution/algorithm.h"
#include "Generic/execution/promise.h"

#include "Generic/projections.h"

#include "Modules/threading/senders/with_lock.h"

UNIQUECOMPONENT(Engine::Serialize::NoParent<Engine::Scene::SceneManager>);

METATABLE_BEGIN(Engine::Scene::SceneManager)
//TODO
//SYNCABLEUNIT_MEMBERS()
READONLY_PROPERTY(entities, entities)
MEMBER(mSceneComponents)
METATABLE_END(Engine::Scene::SceneManager)

static Engine::Threading::DataMutex::Lock static_lock(Engine::Scene::SceneManager *mgr)
{
    return mgr->mutex(Engine::AccessMode::WRITE).lock();
}

SERIALIZETABLE_BEGIN(Engine::Scene::SceneManager,
    Engine::Serialize::CallableGuard<&static_lock>)
FIELD(mEntities,
    Serialize::ParentCreator<&Engine::Scene::SceneManager::readNonLocalEntity, &Engine::Scene::SceneManager::writeEntity>,
    Serialize::RequestPolicy::no_requests)
FIELD(mSceneComponents, Serialize::ControlledConfig<KeyCompare<std::unique_ptr<Engine::Scene::SceneComponentBase>>>)
SERIALIZETABLE_END(Engine::Scene::SceneManager)

namespace Engine {
namespace Scene {

    SceneManager::SceneManager(App::Application &app)
        : SyncableUnitEx(Serialize::SCENE_MANAGER)
        , VirtualScope(app)
        , mSceneComponents(*this)
        , mMutex("SceneData")
        , mFrameClock(std::chrono::steady_clock::now())
        , mSceneClock(mClock.now())
    {
    }

    SceneManager::~SceneManager()
    {
    }

    std::string_view SceneManager::key() const
    {
        return "SceneManager";
    }

    Threading::Task<bool> SceneManager::init()
    {
        for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
            if (!co_await component->callInit())
                co_return false;
        }

        co_return true;
    }

    Threading::Task<void> SceneManager::finalize()
    {
        clear();

        for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
            co_await component->callFinalize();
        }
    }

    SceneComponentBase &SceneManager::getComponent(size_t i)
    {
        return mSceneComponents.get(i);
    }

    size_t SceneManager::getComponentCount()
    {
        return mSceneComponents.size();
    }

    Threading::DataMutex::Moded SceneManager::mutex(AccessMode mode)
    {
        return mMutex(mode);
    }

    void SceneManager::updateFrame()
    {
        std::chrono::microseconds frameTimeSinceLastFrame = mFrameClock.tick(std::chrono::steady_clock::now());
        std::chrono::microseconds sceneTimeSinceLastFrame = mSceneClock.tick(mClock.now());

        for (const std::unique_ptr<SceneComponentBase> &comp : mSceneComponents) {
            comp->updateFrame(frameTimeSinceLastFrame, sceneTimeSinceLastFrame);
        }
    }

    Entity::EntityPtr SceneManager::findEntity(const std::string &name)
    {
        auto it = std::ranges::find(mEntities, name, projectionKey);
        if (it == mEntities.end()) {
            return {};
        }
        return &*it;
    }

    std::string SceneManager::generateUniqueName()
    {
        static size_t itemCount = 0;
        return "Madgine_AutoGen_Name_"s + std::to_string(++itemCount);
    }

    void SceneManager::remove(Entity::Entity *e)
    {
        if (e->isLocal()) {
            auto it = std::ranges::find(mLocalEntities, e, projectionAddressOf);
            mLocalEntities.erase(it);
        } else {
            auto it = std::ranges::find(mEntities, e, projectionAddressOf);
            mEntities.erase(it);
        }
    }

    void SceneManager::clear()
    {
        mEntities.clear();
        mLocalEntities.clear();
    }

    void SceneManager::pause()
    {
        if (mClock.mPauseStack++ == 0) {
            mClock.mPauseStart = std::chrono::steady_clock::now();
        }
    }

    bool SceneManager::unpause()
    {
        if (--mClock.mPauseStack == 0) {
            mClock.mPauseAcc += std::chrono::steady_clock::now() - mClock.mPauseStart;
            return true;
        }
        return false;
    }

    bool SceneManager::isPaused() const
    {
        return mClock.mPauseStack > 0;
    }

    const Threading::CustomClock &SceneManager::clock() const
    {
        return mClock;
    }

    Entity::Entity *SceneManager::makeLocalCopy(Entity::Entity &&e)
    {
        return &mLocalEntities.emplace_back(std::move(e), true);
    }

    Serialize::StreamResult SceneManager::readNonLocalEntity(Serialize::FormattedSerializeStream &in, OutRef<SceneManager> &mgr, bool &isLocal, std::string &name)
    {
        STREAM_PROPAGATE_ERROR(in.beginExtendedRead("Entity", 1));
        mgr = *this;
        isLocal = false;
        return read(in, name, "name");
    }

    std::tuple<SceneManager &, bool, std::string> SceneManager::createEntityData(const std::string &name, bool local)
    {
        std::string actualName = name.empty() ? generateUniqueName() : name;

        return make_tuple(std::ref(*this), local, actualName);
    }

    const char *SceneManager::writeEntity(Serialize::FormattedSerializeStream &out, const Entity::Entity &entity) const
    {
        out.beginExtendedWrite("Entity", 1);
        write(out, entity.name(), "name");
        return "Entity";
    }

    Entity::EntityPtr SceneManager::createEntity(const std::string &behavior, const std::string &name,
        const std::function<void(Entity::Entity &)> &init)
    {


        //ValueType behaviorTable /* = app().table()[behavior]*/;
        ObjectPtr table;
        /*if (behaviorTable.is<ObjectPtr>()) {
                table = behaviorTable.as<ObjectPtr>();
            } else*/
        {
            if (!behavior.empty())
                LOG_ERROR("Behaviour \"" << behavior << "\" not found!");
        }
        auto toPtr = [](const typename RefcountedContainer<std::deque<Entity::Entity>>::iterator &it) { return Entity::EntityPtr { &*it }; };
        if (init)
            return toPtr(TupleUnpacker::invokeFlatten(LIFT(mEntities.emplace_init, this), mEntities.end(), init, createEntityData(name, false), table));
        else
            return toPtr(TupleUnpacker::invokeFlatten(emplace, mEntities, mEntities.end(), createEntityData(name, false), table));
    }

    void SceneManager::createEntityAsyncImpl(Serialize::GenericMessageReceiver receiver, const std::string &behavior, const std::string &name, const std::function<void(Entity::Entity &)> &init)
    {

        //ValueType behaviorTable /* = app().table()[behavior]*/;
        ObjectPtr table;
        /*if (behaviorTable.is<ObjectPtr>()) {
                table = behaviorTable.as<ObjectPtr>();
            } else*/
        {
            if (!behavior.empty())
                LOG_ERROR("Behaviour \"" << behavior << "\" not found!");
        }
        auto toPtr = [](const typename RefcountedContainer<std::deque<Entity::Entity>>::iterator &it) { return Entity::EntityPtr { &*it }; };
        if (init)
            Execution::detach(
                Threading::start_with_lock(mutex(AccessMode::WRITE), taskQueue(),
                    TupleUnpacker::invokeFlatten(LIFT(mEntities.emplace_init_async, this), mEntities.end(), init, createEntityData(name, false), table)
                        | Execution::then(std::move(toPtr)))
                | Execution::then_receiver(std::move(receiver)));
        else
            Execution::detach(
                Threading::start_with_lock(mutex(AccessMode::WRITE), taskQueue(),
                    TupleUnpacker::invokeFlatten(LIFT(mEntities.emplace_async, this), mEntities.end(), createEntityData(name, false), table)
                        | Execution::then(std::move(toPtr)))
                | Execution::then_receiver(std::move(receiver)));
    }

    Entity::EntityPtr SceneManager::createLocalEntity(const std::string &behavior, const std::string &name)
    {

        //ValueType behaviorTable /* = app().table()[behavior]*/;
        ObjectPtr table;
        /*if (behaviorTable.is<ObjectPtr>()) {
                table = behaviorTable.as<ObjectPtr>();
            } else*/
        {
            if (!behavior.empty())
                LOG_ERROR("Behaviour \"" << behavior << "\" not found!");
        }
        return &*TupleUnpacker::invokeFlatten(emplace, mLocalEntities, mLocalEntities.end(), createEntityData(name, true), table);
    }

    Threading::SignalStub<const RefcountedContainer<std::deque<Entity::Entity>>::iterator &, int> &SceneManager::entitiesSignal()
    {
        return mEntities.observer().signal();
    }

    std::chrono::steady_clock::time_point SceneManager::Clock::get(std::chrono::steady_clock::time_point timepoint) const
    {
        return (mPauseStack > 0 ? mPauseStart : timepoint) - mPauseAcc;
    }

    std::chrono::steady_clock::time_point SceneManager::Clock::revert(std::chrono::steady_clock::time_point timepoint) const
    {
        return timepoint + mPauseAcc + (mPauseStack > 0 ? std::chrono::steady_clock::now() - mPauseStart : 0s);
    }

}
}
