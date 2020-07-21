#include "../../scenelib.h"

#include "entity.h"

#include "componentexception.h"

#include "../scenemanager.h"

#include "entitycomponentcollector.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

METATABLE_BEGIN(Engine::Scene::Entity::Entity)
READONLY_PROPERTY(Components, components)
METATABLE_END(Engine::Scene::Entity::Entity)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Entity)
FIELD(mComponents, Serialize::ParentCreator<&Engine::Scene::Entity::Entity::createComponentTuple, &Engine::Scene::Entity::Entity::storeComponentCreationData>)
SERIALIZETABLE_END(Engine::Scene::Entity::Entity)

namespace Engine {

namespace Scene {
    namespace Entity {
        /*Entity::Entity(const Entity &other, bool local)
            : SerializableUnit(other)
            , mName(other.mName)
            , mLocal(local)
            , mSceneManager(other.mSceneManager)
        {
        }*/

        Entity::Entity(Entity &&other, bool local)
            : SerializableUnit(std::move(other))
            , mName(other.mName)
            , mLocal(local)
            , mComponents(std::move(other.mComponents))
            , mSceneManager(other.mSceneManager)
        {
        }

        Entity::Entity(Entity &&other)
            : Entity(std::move(other), other.mLocal)
        {
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

        const std::string &Entity::key() const
        {
            return mName;
        }

        const std::string &Entity::name() const
        {
            return mName;
        }

        EntityComponentBase *Entity::getComponent(size_t i)
        {
            auto it = mComponents.physical().find(i);
            if (it == mComponents.physical().end())
                return nullptr;
            return it->second.get();
        }

        const EntityComponentBase *Entity::getComponent(size_t i) const
        {
            auto it = mComponents.physical().find(i);
            if (it == mComponents.physical().end())
                return nullptr;
            return it->second.get();
        }

        EntityComponentBase *Entity::getComponent(const std::string_view &name)
        {
            return getComponent(sComponentsByName()[name]);
        }

        const EntityComponentBase *Entity::getComponent(const std::string_view &name) const
        {
            return getComponent(sComponentsByName()[name]);
        }

        EntityComponentBase *Entity::toEntityComponentPtr(const std::pair<const uint32_t, std::unique_ptr<EntityComponentBase>> &p)
        {
            return p.second.get();
        }

        bool Entity::hasComponent(size_t i)
        {
            return mComponents.contains(i);
        }

        bool Entity::hasComponent(const std::string_view &name)
        {
            return hasComponent(sComponentsByName()[name]);
        }

        Future<EntityComponentBase *> Entity::addComponent(const std::string_view &name, const ObjectPtr &table)
        {
            return addComponent(sComponentsByName().at(name), table);
        }

        Future<EntityComponentBase *> Entity::addComponent(size_t i, const ObjectPtr &table)
        {
            auto it = mComponents.physical().find(i);
            if (it != mComponents.physical().end()) {
                return it->second.get();
            } else {
                return mComponents.try_emplace(i, EntityComponentRegistry::getConstructor(i)(table)).execute().then([](auto pib) { return pib.first->second.get(); });
            }
        }

        void Entity::removeComponent(const std::string_view &name)
        {
            auto it = mComponents.find(sComponentsByName().at(name));
            assert(it != mComponents.end());
            mComponents.erase(it);
        }

        std::tuple<uint32_t, std::unique_ptr<EntityComponentBase>> Entity::createComponentTuple(const std::string &name)
        {
            uint32_t i = sComponentsByName().at(name);
            return make_tuple(i, EntityComponentRegistry::getConstructor(i)({}));
        }

        std::tuple<std::pair<const char *, std::string_view>> Entity::storeComponentCreationData(const std::pair<const uint32_t, std::unique_ptr<EntityComponentBase>> &comp) const
        {
            return std::make_tuple(std::make_pair("type", comp.second->key()));
        }

        void Entity::remove()
        {
            mSceneManager.remove(this);
        }

        SceneComponentBase &Entity::getSceneComponent(size_t i, bool init)
        {
            return mSceneManager.getComponent(i, init);
        }

        App::GlobalAPIBase &Entity::getGlobalAPIComponent(size_t i, bool init)
        {
            return mSceneManager.getGlobalAPIComponent(i, init);
        }

        void Entity::swap(Entity &other)
        {
            std::swap(mName, other.mName);
            std::swap(mComponents, other.mComponents);
            SerializableUnit::swap(other);
        }

        EntityComponentBase *Entity::addComponentSimple(const std::string_view &name, const ObjectPtr &table)
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

        void Entity::handleEntityEvent(const typename std::map<uint32_t, std::unique_ptr<EntityComponentBase>>::iterator &it, int op)
        {
            switch (op) {
            case BEFORE | RESET:
                throw "TODO";
            case AFTER | RESET:
                throw "TODO";
            case AFTER | INSERT_ITEM:
                it->second->init(mSceneManager.toEntityPtr(this));
                break;
            case BEFORE | REMOVE_ITEM:
                it->second->finalize(mSceneManager.toEntityPtr(this));
                break;
            }
        }
    }
}
}
