#include "../scenelib.h"

#include "scenemanager.h"

#include "Meta/serialize/serializableids.h"

#include "scenecomponentbase.h"

#include "Madgine/app/application.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "entity/entity.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "entity/entitycomponentlistbase.h"

#include "Meta/serialize/configs/controlled.h"

UNIQUECOMPONENT(Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>);

METATABLE_BEGIN(Engine::Scene::SceneManager)
//TODO
//SYNCABLEUNIT_MEMBERS()
READONLY_PROPERTY(entities, entities)
MEMBER(mSceneComponents)
MEMBER(mAmbientLightColor)
MEMBER(mAmbientLightDirection)
METATABLE_END(Engine::Scene::SceneManager)

SERIALIZETABLE_BEGIN(Engine::Scene::SceneManager)
FIELD(mEntities, Serialize::ParentCreator<&Engine::Scene::SceneManager::entityCreationNames, &Engine::Scene::SceneManager::createNonLocalEntityData, &Engine::Scene::SceneManager::storeEntityCreationData>, Serialize::RequestPolicy::no_requests)
FIELD(mSceneComponents, Serialize::ControlledConfig<KeyCompare<std::unique_ptr<Engine::Scene::SceneComponentBase>>>)
SERIALIZETABLE_END(Engine::Scene::SceneManager)

namespace Engine {
namespace Scene {

    SceneManager::SceneManager(App::Application &app)
        : SyncableUnit(Serialize::SCENE_MANAGER)
        , VirtualScope(app)
        , mSceneComponents(*this)
    {
    }

    SceneManager::~SceneManager()
    {
    }

    Threading::Task<bool> SceneManager::init()
    {
        for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
            if (!co_await component->callInit())
                co_return false;
        }

        mApp.taskQueue()->addRepeatedTask([this]() { update(); }, std::chrono::microseconds { 33 }, this);

        co_return true;
    }

    Threading::Task<void> SceneManager::finalize()
    {
        clear();

        mApp.taskQueue()->removeRepeatedTasks(this);

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

    App::GlobalAPIBase &SceneManager::getGlobalAPIComponent(size_t i)
    {
        return mApp.getGlobalAPIComponent(i);
    }

    Threading::DataMutex &SceneManager::mutex()
    {
        return mMutex;
    }

    void SceneManager::update()
    {
        PROFILE();

        Threading::DataLock lock(mMutex, Threading::AccessMode::WRITE);

        std::chrono::microseconds timeSinceLastFrame = mFrameClock.tick<std::chrono::microseconds>();

        for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
            //PROFILE(component->componentName());
            component->update(timeSinceLastFrame, mPauseStack > 0);
        }
    }

    void SceneManager::updateRender()
    {
        for (const std::unique_ptr<Entity::EntityComponentListBase>& list : mEntityComponentLists) {
            list->updateRender();
        }
    }

    Entity::EntityPtr SceneManager::findEntity(const std::string &name)
    {
        auto it = std::find_if(mEntities.begin(), mEntities.end(), [&](const Entity::Entity &e) {
            return e.key() == name;
        });
        if (it == mEntities.end()) {
            std::terminate();
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
            auto it = std::find_if(mLocalEntities.begin(), mLocalEntities.end(), [=](Entity::Entity &ent) { return &ent == e; });
            mLocalEntities.erase(it);
        } else {
            auto it = std::find_if(mEntities.begin(), mEntities.end(), [=](Entity::Entity &ent) { return &ent == e; });
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
        ++mPauseStack;
    }

    bool SceneManager::unpause()
    {
        return --mPauseStack == 0;
    }

    bool SceneManager::isPaused() const
    {
        return mPauseStack > 0;
    }

    Entity::Entity *SceneManager::makeLocalCopy(Entity::Entity &&e)
    {
        return &mLocalEntities.emplace_back(std::move(e), true);
    }

    const char *SceneManager::entityCreationNames(size_t index)
    {
        assert(index == 0);
        return "name";
    }

    std::tuple<SceneManager &, bool, std::string> SceneManager::createNonLocalEntityData(const std::string &name)
    {
        return createEntityData(name, false);
    }

    std::tuple<SceneManager &, bool, std::string> SceneManager::createEntityData(const std::string &name, bool local)
    {
        std::string actualName = name.empty() ? generateUniqueName() : name;

        return make_tuple(std::ref(*this), local, actualName);
    }

    std::tuple<std::string> SceneManager::storeEntityCreationData(const Entity::Entity &entity) const
    {
        return std::make_tuple(entity.name());
    }

    Future<Entity::EntityPtr> SceneManager::createEntity(const std::string &behavior, const std::string &name,
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
        Future<Pib<typename RefcountedContainer<std::deque<Entity::Entity>>::iterator>> f;
        if (init) {
            f = TupleUnpacker::invokeExpand(&decltype(mEntities)::emplace<SceneManager &, bool, std::string, ObjectPtr>, &mEntities, mEntities.end(), tuple_cat(createEntityData(name, false), std::make_tuple(table))).init(init);
        } else
            f = TupleUnpacker::invokeExpand(&decltype(mEntities)::emplace<SceneManager &, bool, std::string, ObjectPtr>, &mEntities, mEntities.end(), tuple_cat(createEntityData(name, false), std::make_tuple(table)));
        return { f.then([](const typename RefcountedContainer<std::deque<Entity::Entity>>::iterator &it) { return Entity::EntityPtr { &*it }; }) };
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
        const std::tuple<SceneManager &, bool, std::string> &data = createEntityData(name, true);
        return &*mLocalEntities.emplace(mLocalEntities.end(), std::get<0>(data), std::get<1>(data), std::get<2>(data), table);
    }

    Threading::SignalStub<const RefcountedContainer<std::deque<Entity::Entity>>::iterator &, int> &SceneManager::entitiesSignal()
    {
        return mEntities.observer().signal();
    }
}
}
