#include "../../baselib.h"

#include "entity.h"

#include "componentexception.h"

#include "../scenemanagerbase.h"

#include "../../scripting/types/globalscopebase.h"

#include "entitycomponentbase.h"

#include "../../scripting/datatypes/luatableiterator.h"
#include "../../scripting/datatypes/luatablefieldaccessor.h"

#include "../../app/application.h"

	API_IMPL(Engine::Scene::Entity::Entity, MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId), MAP_F(addComponent), MAP_F(remove
	), /*&enqueueMethod,*/ /*MAP_RO(position, getPosition), MAP_F(getCenter), MAP_F(setObjectVisible)*/);

namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{
			Entity::Entity(const Entity& other, bool local) :
				SerializableUnit(other),
				Scope<Engine::Scene::Entity::Entity, Engine::Scripting::ScopeBase>(other.mSceneManager.app().createTable()),
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

			Entity::Entity(SceneManagerBase &sceneMgr, bool local, const std::string& name, const std::string& behaviour) :
				Scope<Engine::Scene::Entity::Entity, Engine::Scripting::ScopeBase>(sceneMgr.app().createTable()),
				mName(name),
				mLocal(local),
				mSceneManager(sceneMgr)
			{
				setup();
				if (!behaviour.empty())
				{
					ValueType table = sceneMgr.app().table()[behaviour];
					if (table.is<Scripting::LuaTable>())
					{
						for (const std::pair<std::string, ValueType>& p : table.as<Scripting::LuaTable>())
						{
							if (p.second.is<Scripting::LuaTable>())
							{
								addComponent(p.first, p.second.as<Scripting::LuaTable>());
							}
							else
							{
								LOG_WARNING(message("Non-Table value at key \"", "\"!")(p.first));
							}
						}
					}
					else
					{
						LOG_ERROR(message("Behaviour \"", "\" not found!")(behaviour));
					}
				}
			}

			Entity::~Entity()
			{
			}

			void Entity::setup()
			{
				mComponents.connectCallback([](const decltype(mComponents)::const_iterator& it, int op)
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

			void Entity::addComponent(const std::string& name, const Scripting::LuaTable& table)
			{
				addComponentImpl(createComponent(name, table));
			}

			void Entity::removeComponent(const std::string& name)
			{
				auto it = mComponents.find(name);
				assert(it != mComponents.end());
				mComponents.erase(it);
			}

			bool Entity::existsComponent(const std::string& name)
			{
				return sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end();
			}

			std::set<std::string> Entity::registeredComponentNames()
			{
				std::set<std::string> result;

				for (const std::pair<const std::string, ComponentBuilder>& p : sRegisteredComponentsByName())
				{
					result.insert(p.first);
				}

				return result;
			}

			std::unique_ptr<EntityComponentBase> Entity::createComponent(
				const std::string& name, const Scripting::LuaTable& table)
			{
				auto it = sRegisteredComponentsByName().find(name);
				if (it == sRegisteredComponentsByName().end()) {
					typedef std::map<std::string, ComponentBuilder> & (*ComponentGetter)();
					for (const std::pair<const std::string, Plugins::Plugin> &p : sceneMgr().app().pluginMgr()) {
						ComponentGetter getter = (ComponentGetter)p.second.getSymbol("pluginEntityComponents");
						if (getter) {
							it = (*getter)().find(name);
							if (it != (*getter)().end()) {
								return it->second(*this, table);
							}
						}
					}
					throw ComponentException(Exceptions::unknownComponent(name));
				}
				return it->second(*this, table);
			}

			std::tuple<std::unique_ptr<EntityComponentBase>> Entity::createComponentTuple(const std::string& name)
			{
				return make_tuple(createComponent(name));
			}

			EntityComponentBase* Entity::addComponentImpl(std::unique_ptr<EntityComponentBase>&& component)
			{
				if (mComponents.find(component) != mComponents.end())
					throw ComponentException(Exceptions::doubleComponent(component->key()));
				if (&component->getEntity() != this)
					throw ComponentException(Exceptions::corruptData);
				return mComponents.emplace(std::forward<std::unique_ptr<EntityComponentBase>>(component)).first->get();
			}

			std::map<std::string, Entity::ComponentBuilder>& Entity::sRegisteredComponentsByName()
			{
				static std::map<std::string, ComponentBuilder> dummy;
				return dummy;
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

			SceneComponentBase& Entity::getSceneComponent(size_t i)
			{
				return mSceneManager.getComponent(i);
			}

			Scripting::GlobalAPIComponentBase & Entity::getGlobalAPIComponent(size_t i)
			{
				return mSceneManager.getGlobalAPIComponent(i);
			}

			App::Application& Entity::app()
			{
				return mSceneManager.app();
			}

			KeyValueMapList Entity::maps()
			{
				return Scope::maps().merge(mComponents);
			}

			SceneManagerBase& Entity::sceneMgr() const
			{
				return mSceneManager;
			}

			bool Entity::isLocal() const
			{
				return mLocal;
			}
		}
	}
}
