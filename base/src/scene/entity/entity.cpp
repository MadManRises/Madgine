#include "baselib.h"

#include "entity.h"

#include "componentexception.h"

#include "scene/scenemanager.h"

#include "scripting/types/globalscopebase.h"

namespace Engine {

	API_IMPL(Scene::Entity::Entity, MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId), MAP_F(addComponent), MAP_F(remove), /*&enqueueMethod,*/ /*MAP_RO(position, getPosition), MAP_F(getCenter), MAP_F(setObjectVisible)*/);


namespace Scene {
namespace Entity {



Entity::Entity(const Entity &other) :
	SerializableUnit(other.topLevel(), other),
	mName(other.mName),
	mSceneManager(other.mSceneManager)
{
}

Entity::Entity(Entity &&other) :
	SerializableUnit(other.topLevel(), std::forward<Entity>(other)),
	mName(other.mName),	
	mComponents(std::forward<decltype(mComponents)>(other.mComponents)),
	mSceneManager(other.mSceneManager)
{
}

Entity::Entity(SceneManagerBase *sceneMgr, const std::string &name, const std::string &behaviour) :
	SerializableUnit(sceneMgr),
	mName(name),
	mSceneManager(sceneMgr)
{
	if (!behaviour.empty()) {
		ValueType table = Scripting::GlobalScopeBase::getSingleton().table().getValue(behaviour);
		if (table.is<Scripting::LuaTable>()) {
			for (const std::pair<std::string, ValueType> &p : table.as<Scripting::LuaTable>()) {
				if (p.second.is<Scripting::LuaTable>()) {
					addComponent(p.first, p.second.as<Scripting::LuaTable>());
				}
				else {
					LOG_WARNING(message("Non-Table value at key \"", "\"!")(p.first));
				}
			}
		}
		else {
			LOG_ERROR(message("Behaviour \"", "\" not found!")(behaviour));
		}
	}
}

Entity::~Entity()
{	
	finalize();
}

void Entity::finalize()
{
	for (const std::unique_ptr<EntityComponentBase> &comp : mComponents) {
		comp->finalize();
	}
	mComponents.clear();
}

const char *Entity::key() const
{
	return mName.c_str();
}

void Entity::writeCreationData(Serialize::SerializeOutStream &of) const
{
	SerializableUnitBase::writeCreationData(of);
    of << mName/* << getObjectName()*/;
}

EntityComponentBase * Entity::getComponent(const std::string & name)
{
	auto it = mComponents.find(name);
	if (it == mComponents.end())
		return nullptr;
	else
		return it->get();
}

bool Entity::hasComponent(const std::string & name)
{
	return mComponents.contains(name);
}

void Entity::addComponent(const std::string &name, const Scripting::LuaTable &table){
	addComponentImpl(std::get<0>(createComponent(name, table)));
}

void Entity::removeComponent(const std::string & name)
{
	auto it = mComponents.find(name);
	assert(it != mComponents.end());	
	mComponents.erase(it);
}

bool Entity::existsComponent(const std::string &name)
{
    return sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end();
}

std::set<std::string> Entity::registeredComponentNames()
{
	std::set<std::string> result;

	for (const std::pair<const std::string, ComponentBuilder> &p : sRegisteredComponentsByName()) {
		result.insert(p.first);
	}

	return result;
}

std::tuple<std::unique_ptr<EntityComponentBase>> Entity::createComponent(const std::string & name, const Scripting::LuaTable &table)
{
	auto it = sRegisteredComponentsByName().find(name);
	if (it == sRegisteredComponentsByName().end())
		throw ComponentException(Exceptions::unknownComponent(name));
	return std::make_tuple(it->second(*this, table));
}

EntityComponentBase *Entity::addComponentImpl(std::unique_ptr<EntityComponentBase> &&component)
{
    if (mComponents.find(component) != mComponents.end())
        throw ComponentException(Exceptions::doubleComponent(component->key()));
    if (&component->getEntity() != this)
        throw ComponentException(Exceptions::corruptData);
    return mComponents.emplace(std::forward<std::unique_ptr<EntityComponentBase>>(component))->get();
}


void Entity::remove()
{
	mSceneManager->removeLater(this);
}


void Entity::writeState(Serialize::SerializeOutStream &of) const
{
	SerializableUnitBase::writeState(of);
}

void Entity::readState(Serialize::SerializeInStream &ifs)
{
	SerializableUnitBase::readState(ifs);
}

KeyValueMapList Entity::maps()
{
	return Scope::maps().merge(mComponents);
}

SceneManagerBase & Entity::sceneMgr() const
{
	return *mSceneManager;
}

}
}


}
