#include "../scenelib.h"

#include "scenemanager.h"

#include "Modules/serialize/serializableids.h"

#include "scenecomponentbase.h"

#include "Madgine/app/application.h"

#include "Modules/debug/profiler/profile.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/threading/defaulttaskqueue.h"

#include "entity/entity.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>);

METATABLE_BEGIN(Engine::Scene::SceneManager)
//TODO
//SERIALIZABLEUNIT_MEMBERS()
READONLY_PROPERTY(entities, entities)
MEMBER(mSceneComponents)
METATABLE_END(Engine::Scene::SceneManager)

SERIALIZETABLE_BEGIN(Engine::Scene::SceneManager)
FIELD(mEntities, Serialize::ParentCreator<&Engine::Scene::SceneManager::createNonLocalEntityData, &Engine::Scene::SceneManager::storeEntityCreationData>)
FIELD(mSceneComponents, Serialize::ControlledConfig<KeyCompare<std::unique_ptr<Engine::Scene::SceneComponentBase>>>)
SERIALIZETABLE_END(Engine::Scene::SceneManager)

namespace Engine {
namespace Scene {

    SceneManager::SceneManager(App::Application &app)
        : SerializableUnit(Serialize::SCENE_MANAGER)
        , VirtualScope(app)
        , mSceneComponents(*this)
    {
    }

    SceneManager::~SceneManager()
    {
    }

    bool SceneManager::init()
    {
        markInitialized();

        for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
            if (!component->callInit())
                return false;
        }

        mLastFrame = std::chrono::steady_clock::now();

        Threading::DefaultTaskQueue::getSingleton().addRepeatedTask([this]() { update(); }, std::chrono::microseconds { 30 /*0000*/ }, this);

        return true;
    }

    void SceneManager::finalize()
    {
        clear();

        Threading::DefaultTaskQueue::getSingleton().removeRepeatedTasks(this);

        for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
            component->callFinalize();
        }
    }

    decltype(SceneManager::mEntities) &SceneManager::entities()
    {
        return mEntities;
    }

    SceneComponentBase &SceneManager::getComponent(size_t i, bool init)
    {
        return getChild(mSceneComponents.get(i), init);
    }

    size_t SceneManager::getComponentCount()
    {
        return mSceneComponents.size();
    }

    App::GlobalAPIBase &SceneManager::getGlobalAPIComponent(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return mApp.getGlobalAPIComponent(i, init);
    }

    Threading::DataMutex &SceneManager::mutex()
    {
        return mMutex;
    }

    void SceneManager::update()
    {
        PROFILE();

        Threading::DataLock lock(mMutex, Threading::AccessMode::WRITE);

        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration timeSinceLastFrame = now - mLastFrame;
        mLastFrame = now;

        for (const std::unique_ptr<SceneComponentBase> &component : mSceneComponents) {
            //PROFILE(component->componentName());
            component->update(std::chrono::duration_cast<std::chrono::microseconds>(timeSinceLastFrame));
        }

        for (Entity::Entity &entity : mEntities)
            entity.update();
    }

    Entity::EntityPtr SceneManager::findEntity(const std::string &name)
    {
        auto it = std::find_if(mEntities.begin(), mEntities.end(), [&](const Entity::Entity &e) {
            return e.key() == name;
        });
        if (it == mEntities.end()) {
            std::terminate();
        }
        return it;
    }

    std::string SceneManager::generateUniqueName()
    {
        return "Madgine_AutoGen_Name_"s + std::to_string(++mItemCount);
    }

    void SceneManager::remove(Entity::EntityPtr &e)
    {

        if (e->isLocal()) {
            mLocalEntities.erase(e.it());
        } else {
            mEntities.erase(e.it());
        }
    }

    void SceneManager::clear()
    {
        //for (const std::unique_ptr<Entity::EntityComponentListBase> &list : mEntityComponentLists)
        //    list->clear();

        mEntities.clear();
        mLocalEntities.clear();
    }

    /*Entity::Entity *SceneManager::makeLocalCopy(Entity::Entity &e)
    {
        return &mLocalEntities.emplace_back(e, true);
    }*/

    Entity::Entity *SceneManager::makeLocalCopy(Entity::Entity &&e)
    {
        return &*mLocalEntities.emplace(std::move(e), true);
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

    std::tuple<std::pair<const char *, std::string>> SceneManager::storeEntityCreationData(const Entity::Entity &entity) const
    {
        return std::make_tuple(
            std::make_pair("name", entity.name()));
    }

    /*Threading::SignalStub<const decltype(SceneManager::mEntities)::iterator &, int> &SceneManager::entitiesSignal()
    {
        return mEntities.observer().signal();
    }*/

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
        Future<typename GenerationVector<Entity::Entity>::iterator> f;
        if (init)
            f = TupleUnpacker::invokeExpand(&decltype(mEntities)::emplace<SceneManager &, bool, std::string, ObjectPtr>, &mEntities, mEntities.end(), tuple_cat(createEntityData(name, false), std::make_tuple(table))).init(init);
        else
            f = TupleUnpacker::invokeExpand(&decltype(mEntities)::emplace<SceneManager &, bool, std::string, ObjectPtr>, &mEntities, mEntities.end(), tuple_cat(createEntityData(name, false), std::make_tuple(table)));
        return std::move(f);
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
        return mLocalEntities.emplace(std::get<0>(data), std::get<1>(data), std::get<2>(data), table);
    }

    Entity::EntityPtr SceneManager::toEntityPtr(Entity::Entity *e)
    {
        return std::next(mEntities.begin(), e - &mEntities.front());
    }

    Threading::SignalStub<const GenerationVector<Entity::Entity>::iterator &, int> &SceneManager::entitiesSignal()
    {
        return mEntities.observer().signal();
    }
}
}
