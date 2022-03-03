#include "../../scenelib.h"

#include "entitycomponentptr.h"

#include "entity.h"

#include "../scenemanager.h"

#include "entitycomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/serializetable_impl.h"

#include "entitycomponentlistbase.h"

METATABLE_BEGIN(Engine::Scene::Entity::Entity)
NAMED_MEMBER(Name, mName)
READONLY_PROPERTY(Components, components)
METATABLE_END(Engine::Scene::Entity::Entity)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Entity)
FIELD(mComponents, Serialize::ParentCreator<&Engine::Scene::Entity::Entity::readComponent, &Engine::Scene::Entity::Entity::writeComponent, &Engine::Scene::Entity::Entity::clearComponents>)
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

        EntityComponentPtr<EntityComponentBase> Entity::getComponent(std::string_view name)
        {
            return getComponent(EntityComponentRegistry::sComponentsByName().at(name));
        }

        EntityComponentPtr<const EntityComponentBase> Entity::getComponent(std::string_view name) const
        {
            return getComponent(EntityComponentRegistry::sComponentsByName().at(name));
        }

        bool Entity::hasComponent(size_t i)
        {
            return mComponents.contains(i);
        }

        bool Entity::hasComponent(std::string_view name)
        {
            return hasComponent(EntityComponentRegistry::sComponentsByName().at(name));
        }

        EntityComponentPtr<EntityComponentBase> Entity::addComponent(std::string_view name, const ObjectPtr &table)
        {
            return addComponent(EntityComponentRegistry::sComponentsByName().at(name), table);
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

        void Entity::removeComponent(std::string_view name)
        {
            removeComponent(EntityComponentRegistry::sComponentsByName().at(name));
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

        void Entity::relocateComponent(EntityComponentHandle<EntityComponentBase> newIndex)
        {
            auto it = mComponents.find(newIndex.mType);
            it->mHandle.mIndex = newIndex.mIndex;
        }

        void Entity::remove()
        {
            mSceneManager.remove(this);
        }

        Serialize::StreamResult Entity::readComponent(Serialize::FormattedSerializeStream &in, EntityComponentOwningHandle<EntityComponentBase> &handle)
        {
            STREAM_PROPAGATE_ERROR(in.beginExtendedRead("Component", 1));
            std::string name;
            STREAM_PROPAGATE_ERROR(read(in, name, "name"));
            uint32_t i = EntityComponentRegistry::sComponentsByName().at(name);
            handle = mSceneManager.entityComponentList(i).emplace({}, this);
            return {};
        }

        void Entity::writeComponent(Serialize::FormattedSerializeStream &out, const EntityComponentOwningHandle<EntityComponentBase> &comp) const
        {
            out.beginExtendedWrite("Component", 1);
            for (const auto &p : EntityComponentRegistry::sComponentsByName()) {
                if (p.second == comp.mHandle.mType) {
                    write(out, p.first, "name");
                    return;
                }
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

        SceneManager &Entity::sceneMgr()
        {
            return mSceneManager;
        }

        const SceneManager &Entity::sceneMgr() const
        {
            return mSceneManager;
        }

        EntityComponentPtr<EntityComponentBase> Entity::Helper::operator()(const EntityComponentOwningHandle<EntityComponentBase> &p)
        {
            return { p, &mEntity->mSceneManager };
        }
    }
}
}
