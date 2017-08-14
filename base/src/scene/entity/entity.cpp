#include "baselib.h"

#include "entity.h"

#include "componentexception.h"

#include "scene/scenemanager.h"

#include "scripting/types/globalscope.h"

namespace Engine {

	API_IMPL(Engine::Scene::Entity::Entity, MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId), MAP_F(addComponent), MAP_F(remove), /*&enqueueMethod,*/ MAP_RO(position, getPosition), MAP_F(getCenter), MAP_F(setObjectVisible));


namespace Scene {
namespace Entity {



Entity::Entity(const Entity &other) :
	SerializableUnitBase(other.topLevel()),
	mName(other.mName),
	mSceneManager(other.mSceneManager)
{

	//TODO copy Components

}

Entity::Entity(Entity &&other) :
	SerializableUnitBase(other.topLevel()),
	mName(other.mName),	
	mComponents(std::forward<decltype(mComponents)>(other.mComponents)),
	mSceneManager(other.mSceneManager)
{

}

Entity::Entity(SceneManagerBase *sceneMgr, const std::string &name) :
	SerializableUnitBase(sceneMgr),
	mName(name),
	mSceneManager(sceneMgr)
{
	
}

Entity::~Entity()
{	

	mComponents.clear();

}

bool Entity::init(const std::string &behaviour)
{
	if (!Scope::init())
		return false;

	if (!behaviour.empty()) {
		ValueType table = Scripting::GlobalScope::getSingleton().table().getValue(behaviour);
		if (!table.is<Scripting::LuaTable>())
			throw 0;
		for (const std::pair<std::string, ValueType> &p : table.as<Scripting::LuaTable>()) {
			if (p.second.is<Scripting::LuaTable>()) {
				addComponent(p.first, p.second.as<Scripting::LuaTable>());
			}
			else {
				LOG_WARNING(message("Non-Table value at key \"", "\"!")(p.first));
			}
		}
	}

	return true;
}


void Entity::onLoad()
{
    callMethodIfAvailable("onLoad");
}

const char *Entity::key() const
{
	return mName.c_str();
}

void Entity::writeCreationData(Serialize::SerializeOutStream &of) const
{
	SerializableUnitBase::writeCreationData(of);
    of << mName << getObjectName();
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


size_t Entity::getSize() const
{
	return sizeof(Entity);
}

void Entity::remove()
{
	mSceneManager->removeLater(this);
}


void Entity::writeState(Serialize::SerializeOutStream &of) const
{
	of << ValueType(getPosition());

	of << getOrientation();

	of << getScale();
	
	SerializableUnitBase::writeState(of);

}

void Entity::readState(Serialize::SerializeInStream &ifs)
{
	Vector3 v;
	ifs >> v;
	setPosition(v);

	std::array<float, 4> q;
	ifs >> q;
	setOrientation(q);

	ifs >> v;
	setScale(v);

	SerializableUnitBase::readState(ifs);
}

std::array<float, 2> Entity::getCenter2D() const {
	Vector3 c = getCenter();
	return{ { c.x, c.z } };
}

std::array<float, 2> Entity::getPosition2D() const {
	Vector3 p = getPosition();
	return{ { p.x, p.z } };
}

KeyValueMapList Entity::maps()
{
	return Scope::maps().merge(mComponents);
}

}
}


}
