#include "../../baselib.h"

#include "entity.h"

#include "componentexception.h"

#include "../scenemanager.h"

#include "entitycomponentbase.h"

#include "Interfaces/scripting/datatypes/luatableiterator.h"

#include "entitycomponentcollector.h"

#include "Interfaces/scripting/types/api.h"
#include "Interfaces/generic/keyvalueiterate.h"



namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{
			Entity::Entity(const Entity& other, bool local) :
				SerializableUnit(other),
				Scope<Engine::Scene::Entity::Entity, Engine::Scripting::ScopeBase>(&other.mSceneManager),
				mName(other.mName),
				mLocal(local),
				mSceneManager(other.mSceneManager)
			{
				setup();
			}

			Entity::Entity(Entity&& other, bool local) :
				SerializableUnit(std::forward<Entity>(other)),
				Scope<Engine::Scene::Entity::Entity, Engine::Scripting::ScopeBase>(std::forward<Entity>(other)),
				mName(other.mName),
				mLocal(local),
				mComponents(std::forward<decltype(mComponents)>(other.mComponents)),
				mSceneManager(other.mSceneManager)
			{
				for (const std::unique_ptr<EntityComponentBase>& comp : mComponents)
				{
					comp->moveToEntity(this);
				}
				setup();
			}

			Entity::Entity(SceneManager &sceneMgr, bool local, const std::string& name, const Scripting::LuaTable& behaviour) :
				Scope<Engine::Scene::Entity::Entity, Engine::Scripting::ScopeBase>(&sceneMgr),
				mName(name),
				mLocal(local),
				mSceneManager(sceneMgr)
			{
				setup();
				if (behaviour)
				{
					for (const std::pair<std::string, ValueType>& p : behaviour)
					{
						if (p.second.is<Scripting::LuaTable>())
						{
							addComponent(p.first, p.second.as<Scripting::LuaTable>());
						}
						else
						{
							LOG_WARNING(Database::message("Non-Table value at key \"", "\"!")(p.first));
						}
					}
				}
			}

			Entity::~Entity()
			{
			}

			void Entity::setup()
			{
				mComponents.signal().connect([](const decltype(mComponents)::const_iterator& it, int op)
				{
					using namespace Serialize;
					switch (op)
					{
					case BEFORE | RESET:
						break;
					case AFTER | RESET:
						break;
					case INSERT_ITEM:
						(*it)->init();
						break;
					case BEFORE | REMOVE_ITEM:
						(*it)->finalize();
						break;
					}
				});
			}

			const char* Entity::key() const
			{
				return mName.c_str();
			}

			void Entity::writeCreationData(Serialize::SerializeOutStream& of) const
			{
				SerializableUnitBase::writeCreationData(of);
				of << mName;
			}

			EntityComponentBase* Entity::getComponent(const std::string& name)
			{
				auto it = mComponents.find(name);
				if (it == mComponents.end())
					throw 0;
				return it->get();
			}

			bool Entity::hasComponent(const std::string& name)
			{
				return mComponents.contains(name);
			}

			EntityComponentBase *Entity::addComponent(const std::string& name, const Scripting::LuaTable& table)
			{
				auto it = mComponents.find(name);
				if (it != mComponents.end())
				{
					return it->get();
				}
				else {
					return mComponents.emplace(EntityComponentCollector::createComponent(*this, name, table)).first->get();
				}
			}

			void Entity::removeComponent(const std::string& name)
			{
				auto it = mComponents.find(name);
				assert(it != mComponents.end());
				mComponents.erase(it);
			}



			
			std::tuple<std::unique_ptr<EntityComponentBase>> Entity::createComponentTuple(const std::string& name)
			{
				return make_tuple(EntityComponentCollector::createComponent(*this, name));
			}			


			void Entity::remove()
			{
				mSceneManager.removeLater(this);
			}


			void Entity::writeState(Serialize::SerializeOutStream& of) const
			{
				SerializableUnitBase::writeState(of);
			}

			void Entity::readState(Serialize::SerializeInStream& ifs)
			{
				SerializableUnitBase::readState(ifs);
			}

			SceneComponentBase& Entity::getSceneComponent(size_t i, bool init)
			{
				return mSceneManager.getComponent(i, init);
			}

			App::GlobalAPIComponentBase & Entity::getGlobalAPIComponent(size_t i, bool init)
			{
				return mSceneManager.getGlobalAPIComponent(i, init);
			}

			App::Application& Entity::app(bool init)
			{
				return mSceneManager.app(init);
			}

			KeyValueMapList Entity::maps()
			{
				return Scope::maps().merge(mComponents, MAP_F(addComponent), MAP_F(remove), MAP_RO(Synced, isSynced), MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId));
			}

			SceneManager& Entity::sceneMgr(bool init) const
			{
				return mSceneManager.getSelf(init);
			}

			bool Entity::isLocal() const
			{
				return mLocal;
			}
		}
	}
}