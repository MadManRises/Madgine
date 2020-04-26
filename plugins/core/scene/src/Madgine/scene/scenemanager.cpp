#include "../scenelib.h"

#include "scenemanager.h"

#include "Modules/serialize/serializableids.h"

#include "scenecomponentbase.h"

#include "Madgine/app/application.h"

#include "Modules/debug/profiler/profile.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/threading/defaulttaskqueue.h"

#include "entity/entity.h"

UNIQUECOMPONENT(Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>);

METATABLE_BEGIN(Engine::Scene::SceneManager)
//TODO
//SERIALIZABLEUNIT_MEMBERS()
READONLY_PROPERTY(entities, entities)
MEMBER(mSceneComponents)
METATABLE_END(Engine::Scene::SceneManager)

SERIALIZETABLE_BEGIN(Engine::Scene::SceneManager)
FIELD(mEntities, Serialize::ParentCreator<&Engine::Scene::SceneManager::createNonLocalEntityData>)
FIELD(mSceneComponents)
SERIALIZETABLE_END(Engine::Scene::SceneManager)

RegisterType(Engine::Scene::SceneManager)
    RegisterType(Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>)

        namespace Engine
{
    namespace Scene {

        SceneManager::SceneManager(App::Application &app)
            : SerializableUnit(Serialize::SCENE_MANAGER)
            , VirtualScope(app)
            , mApp(app)
            , mItemCount(0)
            , mSceneComponents(*this)
        {
        }

        Engine::Scene::SceneManager::~SceneManager()
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

            Threading::DefaultTaskQueue::getSingleton().addRepeatedTask([this]() { update(); }, std::chrono::microseconds { 10000 }, this);

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
            SceneComponentBase &comp = mSceneComponents.get(i);
            if (init) {
                checkInitState();
                comp.callInit();
            }
            return comp.getSelf(init);
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

        SceneManager &SceneManager::getSelf(bool init)
        {
            if (init) {
                checkDependency();
            }
            return *this;
        }

        const MadgineObject *SceneManager::parent() const
        {
            return &mApp;
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
                component->update(std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceLastFrame));
            }

            removeQueuedEntities();
        }

        Entity::Entity *SceneManager::findEntity(const std::string &name)
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
            return "Madgine_AutoGen_Name_"s + std::to_string(++mItemCount);
        }

        void SceneManager::removeLater(Entity::Entity *e)
        {
            mEntityRemoveQueue.push_back(e);
        }

        void SceneManager::clear()
        {
            mEntities.clear();
            mLocalEntities.clear();
            mEntityRemoveQueue.clear();
        }

        Entity::Entity *SceneManager::makeLocalCopy(Entity::Entity &e)
        {
            return &mLocalEntities.emplace_back(e, true);
        }

        Entity::Entity *SceneManager::makeLocalCopy(Entity::Entity &&e)
        {
            return &mLocalEntities.emplace_back(std::move(e), true);
        }

        std::tuple<SceneManager &, bool, std::string> SceneManager::createNonLocalEntityData(const std::string &name)
        {
            return createEntityData(name, false);
        }

        std::tuple<SceneManager &, bool, std::string> SceneManager::createEntityData(
            const std::string &name, bool local)
        {
            std::string actualName = name.empty() ? generateUniqueName() : name;

            return make_tuple(std::ref(*this), local, actualName);
        }

        /*Threading::SignalStub<const decltype(SceneManager::mEntities)::iterator &, int> &SceneManager::entitiesSignal()
    {
        return mEntities.observer().signal();
    }*/

        Entity::Entity *SceneManager::createEntity(const std::string &behavior, const std::string &name,
            const std::function<void(Entity::Entity &)> &init)
        {
            //ValueType behaviorTable /* = app().table()[behavior]*/;
            ObjectPtr table;
            /*if (behaviorTable.is<ObjectPtr>()) {
                table = behaviorTable.as<ObjectPtr>();
            } else*/ {
                if (!behavior.empty())
                    LOG_ERROR("Behaviour \"" << behavior << "\" not found!");
            }
            if (init)
                TupleUnpacker::invokeFromTuple(&decltype(mEntities)::emplace_init<decltype(init), SceneManager &, bool, std::string, ObjectPtr>, tuple_cat(std::make_tuple(&mEntities, init, mEntities.end()), createEntityData(name, false), std::make_tuple(table)));
            else
                TupleUnpacker::invokeFromTuple(&decltype(mEntities)::emplace<SceneManager &, bool, std::string, ObjectPtr>, tuple_cat(std::make_tuple(&mEntities, mEntities.end()), createEntityData(name, false), std::make_tuple(table)));
            return &mEntities.back();
        }

        Entity::Entity *SceneManager::createLocalEntity(const std::string &behavior, const std::string &name)
        {
            //ValueType behaviorTable /* = app().table()[behavior]*/;
            ObjectPtr table;
            /*if (behaviorTable.is<ObjectPtr>()) {
                table = behaviorTable.as<ObjectPtr>();
            } else*/ {
                if (!behavior.empty())
                    LOG_ERROR("Behaviour \"" << behavior << "\" not found!");
            }
            const std::tuple<SceneManager &, bool, std::string> &data = createEntityData(name, true);
            return &mLocalEntities.emplace_back(std::get<0>(data), std::get<1>(data), std::get<2>(data), table);
        }

        void SceneManager::removeQueuedEntities()
        {
            std::list<Entity::Entity *>::iterator it = mEntityRemoveQueue.begin();

            auto find = [&](const Entity::Entity &ent) { return &ent == *it; };

            while (it != mEntityRemoveQueue.end()) {
                if ((*it)->isLocal()) {
                    auto ent = std::find_if(mLocalEntities.begin(), mLocalEntities.end(), find);
                    if (ent != mLocalEntities.end()) {
                        mLocalEntities.erase(ent);
                    } else {
                        std::terminate();
                    }
                } else {
                    auto ent = std::find_if(mEntities.begin(), mEntities.end(), find);
                    if (ent != mEntities.end()) {
                        mEntities.erase(ent);
                    } else {
                        std::terminate();
                    }
                }

                it = mEntityRemoveQueue.erase(it);
            }
        }

        Threading::SignalStub<const std::list<Engine::Scene::Entity::Entity>::iterator &, int> &Engine::Scene::SceneManager::entitiesSignal()
        {
            return mEntities.observer().signal();
        }
    }
}
