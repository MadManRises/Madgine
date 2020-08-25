#include "../../scenelib.h"

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
            while (!mComponents.empty())
                removeComponent(mComponents.begin()->first);
        }

        Entity &Entity::operator=(Entity &&other)
        {
            assert(&mSceneManager == &other.mSceneManager);
            SerializableUnit::operator=(std::move(other));
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

        EntityComponentPtr<EntityComponentBase> Entity::getComponent(size_t i, const EntityPtr &self)
        {
            auto it = mComponents.physical().find(i);
            if (it == mComponents.physical().end())
                return {};
            return { { mSceneManager.entityComponentList(i)->copy(it->second.mIndex) }, i, self };
        }

        EntityComponentPtr<const EntityComponentBase> Entity::getComponent(size_t i, const EntityPtr &self) const
        {
            auto it = mComponents.physical().find(i);
            if (it == mComponents.physical().end())
                return {};
            return { { mSceneManager.entityComponentList(i)->copy(it->second.mIndex) }, i, self };
        }

        EntityComponentPtr<EntityComponentBase> Entity::getComponent(const std::string_view &name, const EntityPtr &self)
        {
            return getComponent(sComponentsByName()[name], self);
        }

        EntityComponentPtr<const EntityComponentBase> Entity::getComponent(const std::string_view &name, const EntityPtr &self) const
        {
            return getComponent(sComponentsByName()[name], self);
        }

        EntityComponentBase *Entity::toEntityComponentPtr(const std::pair<const uint32_t, EntityComponentOwningHandle<EntityComponentBase>> &p)
        {
            return mSceneManager.entityComponentList(p.first).get(p.second.mIndex);
        }

        bool Entity::hasComponent(size_t i)
        {
            return mComponents.contains(i);
        }

        bool Entity::hasComponent(const std::string_view &name)
        {
            return hasComponent(sComponentsByName()[name]);
        }

        Future<EntityComponentPtr<EntityComponentBase>> Entity::addComponent(const std::string_view &name, const EntityPtr &self, const ObjectPtr &table)
        {
            return addComponent(sComponentsByName().at(name), self, table);
        }

        Future<EntityComponentPtr<EntityComponentBase>> Entity::addComponent(size_t i, const EntityPtr &self, const ObjectPtr &table)
        {
            auto it = mComponents.physical().find(i);
            if (it != mComponents.physical().end()) {
                return EntityComponentPtr<EntityComponentBase> { { mSceneManager.entityComponentList(i)->copy(it->second.mIndex) }, i, self };
            } else {
                return mComponents.try_emplace(i, mSceneManager.entityComponentList(i).emplace(table)).execute().then([=](auto pib) {
                    return EntityComponentPtr<EntityComponentBase> { { self.sceneMgr()->entityComponentList(i)->copy(pib.first->second.mIndex) }, i, self };
                });
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
            mSceneManager.entityComponentList(i).erase(it->second.mIndex);
            mComponents.erase(it);
        }

        std::tuple<uint32_t, EntityComponentOwningHandle<EntityComponentBase>> Entity::createComponentTuple(const std::string &name)
        {
            uint32_t i = sComponentsByName().at(name);
            return std::make_tuple(i, mSceneManager.entityComponentList(i).emplace({}));
        }

        std::tuple<std::pair<const char *, std::string_view>> Entity::storeComponentCreationData(const std::pair<const uint32_t, EntityComponentOwningHandle<EntityComponentBase>> &comp) const
        {
            for (const auto &p : sComponentsByName()) {
                if (p.second == comp.first)
                    return std::make_tuple(std::make_pair("type", p.first));
            }
            throw 0;
        }

        void Entity::remove()
        {
            EntityPtr p = mSceneManager.toEntityPtr(this);
            mSceneManager.remove(p);
        }

        SceneComponentBase &Entity::getSceneComponent(size_t i, bool init)
        {
            return mSceneManager.getComponent(i, init);
        }

        App::GlobalAPIBase &Entity::getGlobalAPIComponent(size_t i, bool init)
        {
            return mSceneManager.getGlobalAPIComponent(i, init);
        }

        SceneManager &Entity::sceneMgr(bool init) const
        {
            return mSceneManager.getSelf(init);
        }

        bool Entity::isLocal() const
        {
            return mLocal;
        }

        void Entity::handleEntityEvent(const typename std::map<uint32_t, EntityComponentOwningHandle<EntityComponentBase>>::iterator &it, int op)
        {
            switch (op) {
            case BEFORE | RESET:
                throw "TODO";
            case AFTER | RESET:
                throw "TODO";
            case AFTER | EMPLACE:
                mSceneManager.entityComponentList(it->first).get(it->second.mIndex)->init(mSceneManager.toEntityPtr(this));
                break;
            case BEFORE | ERASE:
                mSceneManager.entityComponentList(it->first).get(it->second.mIndex)->finalize(mSceneManager.toEntityPtr(this));
                break;
            }
        }

        void Entity::update()
        {
            for (const std::pair<const uint32_t, EntityComponentOwningHandle<EntityComponentBase>>& p : mComponents) {
                mSceneManager.entityComponentList(p.first)->update(p.second.mIndex);
            }
        }
    }
}
}
