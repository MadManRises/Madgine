#include "../../scenelib.h"

#include "entity.h"

#include "componentexception.h"

#include "../scenemanager.h"

#include "entitycomponentcollector.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Scene::Entity::Entity)
METATABLE_END(Engine::Scene::Entity::Entity)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Entity)
FIELD(mComponents, Serialize::ParentCreator<&Engine::Scene::Entity::Entity::createComponentTuple>)
SERIALIZETABLE_END(Engine::Scene::Entity::Entity)

RegisterType(Engine::Scene::Entity::Entity);

namespace Engine {

namespace Scene {
    namespace Entity {
        Entity::Entity(const Entity &other, bool local)
            : SerializableUnit(other)
            , mName(other.mName)
            , mLocal(local)
            , mSceneManager(other.mSceneManager)
        {
        }

        Entity::Entity(Entity &&other, bool local)
            : SerializableUnit(std::move(other))
            , mName(other.mName)
            , mLocal(local)
            , mComponents(std::move(other.mComponents))
            , mSceneManager(other.mSceneManager)
        {
            for (const std::unique_ptr<EntityComponentBase> &comp : mComponents.physical()) {
                comp->moveToEntity(this);
            }
        }

        Entity::Entity(SceneManager &sceneMgr, bool local, const std::string &name, const ObjectPtr &behaviour)
            : mName(name)
            , mLocal(local)
            , mSceneManager(sceneMgr)
        {
            if (behaviour) {
                /*for (const std::pair<std::string, ValueType> &p : behaviour) {
					if (p.second.is<Scripting::LuaTable>()) {
                        addComponent(p.first, p.second.as<Scripting::LuaTable>());
                    } else {
                        LOG_WARNING("Non-Table value at key \"" << p.first << "\"!");
                    }
                }*/
                throw "Todo";
            }
        }

        Entity::~Entity()
        {
        }

        const char *Entity::key() const
        {
            return mName.c_str();
        }

        void Entity::writeCreationData(Serialize::SerializeOutStream &of) const
        {
            SerializableUnitBase::writeCreationData(of);
            of << mName;
        }

        EntityComponentBase *Entity::getComponent(const std::string &name)
        {
            auto it = mComponents.physical().find(name);
            if (it == mComponents.physical().end())
                return nullptr;
            return it->get();
        }

        bool Entity::hasComponent(const std::string &name)
        {
            return mComponents.contains(name);
        }

        EntityComponentBase *Entity::addComponent(const std::string &name, const ObjectPtr &table)
        {
            auto it = mComponents.physical().find(name);
            if (it != mComponents.physical().end()) {
                return it->get();
            } else {
                return mComponents.emplace(EntityComponentCollector::createComponent(*this, name, table)).first->get();
            }
        }

        void Entity::removeComponent(const std::string &name)
        {
            auto it = mComponents.find(name);
            assert(it != mComponents.end());
            mComponents.erase(it);
        }

        std::tuple<std::unique_ptr<EntityComponentBase>> Entity::createComponentTuple(const std::string &name)
        {
            return make_tuple(EntityComponentCollector::createComponent(*this, name));
        }

        void Entity::remove()
        {
            mSceneManager.removeLater(this);
        }

        SceneComponentBase &Entity::getSceneComponent(size_t i, bool init)
        {
            return mSceneManager.getComponent(i, init);
        }

        App::GlobalAPIBase &Entity::getGlobalAPIComponent(size_t i, bool init)
        {
            return mSceneManager.getGlobalAPIComponent(i, init);
        }

        EntityComponentBase *Entity::addComponentSimple(const std::string &name, const ObjectPtr &table)
        {
            return addComponent(name, table);
        }

        SceneManager &Entity::sceneMgr(bool init) const
        {
            return mSceneManager.getSelf(init);
        }

        bool Entity::isLocal() const
        {
            return mLocal;
        }

        void EntityComponentObserver::operator()(const typename KeyValueSet<std::unique_ptr<EntityComponentBase>>::iterator &it, int op)
        {
            switch (op) {
            case BEFORE | RESET:
                throw "TODO";
            case AFTER | RESET:
                throw "TODO";
            case AFTER | INSERT_ITEM:
                (*it)->init();
                break;
            case BEFORE | REMOVE_ITEM:
                (*it)->finalize();
                break;
            }
        }
    }
}
}