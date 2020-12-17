#include "../../scenelib.h"

#include "entitycomponentptr.h"

#include "entity.h"

#include "componentexception.h"

#include "../scenemanager.h"

#include "entitycomponentcollector.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "entitycomponentserialize.h"

METATABLE_BEGIN(Engine::Scene::Entity::Entity)
READONLY_PROPERTY(Components, components)
METATABLE_END(Engine::Scene::Entity::Entity)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Entity)
FIELD(mComponents, Serialize::ParentCreator<&Engine::Scene::Entity::Entity::createComponentTuple, &Engine::Scene::Entity::Entity::storeComponentCreationData, &Engine::Scene::Entity::Entity::clearComponents>)
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
            : SyncableUnit(std::move(other))
            , mName(std::move(other.mName))
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
            clearComponents();
        }

        Entity &Entity::operator=(Entity &&other)
        {
            assert(&mSceneManager == &other.mSceneManager);
            SerializableUnitBase::operator=(std::move(other));
            mName = std::move(other.mName);
            mComponents = std::move(other.mComponents);
            return *this;
        }

        const std::string &Entity::key() const
        {
            return mName;
        }

        const std::string &Entity::name() const
        {
            return mName;
        }

        EntityComponentPtr<EntityComponentBase> Entity::getComponent(uint32_t i)
        {
            auto it = mComponents.physical().find(i);
            if (it == mComponents.physical().end())
                return {};
            return { *it, &mSceneManager };
        }

        EntityComponentPtr<const EntityComponentBase> Entity::getComponent(uint32_t i) const
        {
            auto it = mComponents.physical().find(i);
            if (it == mComponents.physical().end())
                return {};
            return { { *it }, &mSceneManager };
        }

        EntityComponentPtr<EntityComponentBase> Entity::getComponent(const std::string_view &name)
        {
            return getComponent(sComponentsByName()[name]);
        }

        EntityComponentPtr<const EntityComponentBase> Entity::getComponent(const std::string_view &name) const
        {
            return getComponent(sComponentsByName()[name]);
        }

        bool Entity::hasComponent(size_t i)
        {
            return mComponents.contains(i);
        }

        bool Entity::hasComponent(const std::string_view &name)
        {
            return hasComponent(sComponentsByName()[name]);
        }

        EntityComponentPtr<EntityComponentBase> Entity::addComponent(const std::string_view &name, const ObjectPtr &table)
        {
            return addComponent(sComponentsByName().at(name), table);
        }

        EntityComponentPtr<EntityComponentBase> Entity::addComponent(size_t i, const ObjectPtr &table)
        {
            auto it = mComponents.physical().find(i);
            if (it != mComponents.physical().end()) {
                return { *it, &mSceneManager };
            } else {
                auto pib = mComponents.emplace(mSceneManager.entityComponentList(i).emplace(table, this));
                return EntityComponentPtr<EntityComponentBase> { *pib.first, &mSceneManager };
            }
        }

        void Entity::removeComponent(const std::string_view &name)
        {
            removeComponent(sComponentsByName().at(name));
        }

        void Entity::removeComponent(size_t i)
        {
            auto it = mComponents.find(i);
            assert(it != mComponents.physical().end());
            mSceneManager.entityComponentList(i).erase(*it);
            mComponents.erase(it);
        }

        void Entity::clearComponents()
        {
            while (!mComponents.empty())
                removeComponent(mComponents.begin()->mHandle.mType);
        }

        void Entity::remove()
        {
            mSceneManager.remove(this);
        }

        std::tuple<EntityComponentOwningHandle<EntityComponentBase>> Entity::createComponentTuple(const std::string &name)
        {
            uint32_t i = sComponentsByName().at(name);
            return std::make_tuple(mSceneManager.entityComponentList(i).emplace({}, this));
        }

        std::tuple<std::pair<const char *, std::string_view>> Entity::storeComponentCreationData(const EntityComponentOwningHandle<EntityComponentBase> &comp) const
        {
            for (const auto &p : sComponentsByName()) {
                if (p.second == comp.mHandle.mType)
                    return std::make_tuple(std::make_pair("type", p.first));
            }
            throw 0;
        }

        bool Entity::isLocal() const
        {
            return mLocal;
        }

        void Entity::handleEntityEvent(const typename std::set<EntityComponentOwningHandle<EntityComponentBase>>::iterator &it, int op)
        {
            switch (op) {
            case BEFORE | RESET:
                throw "TODO";
            case AFTER | RESET:
                throw "TODO";
            case AFTER | EMPLACE:
                mSceneManager.entityComponentList(it->mHandle.mType).init(*it, this);
                break;
            case BEFORE | ERASE:
                mSceneManager.entityComponentList(it->mHandle.mType).finalize(*it, this);
                break;
            }
        }

        EntityComponentPtr<EntityComponentBase> Entity::Helper::operator()(EntityComponentOwningHandle<EntityComponentBase> p)
        {
            return { p, &mEntity->mSceneManager };
        }
    }
}
}
