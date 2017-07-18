#include "baselib.h"

#include "entity.h"

#include "masks.h"

#include "componentexception.h"

#include "scene/scenemanager.h"

#include "serialize/streams/serializestream.h"


#include "scripting/parsing/scriptparser.h"


namespace Engine {

	API_IMPL(Engine::Scene::Entity::Entity, MAP_F(addComponent), MAP_F(remove), /*&enqueueMethod,*/ MAP_RO(position, getPosition), MAP_F(getCenter), MAP_F(setObjectVisible));


namespace Scene {
namespace Entity {



Entity::Entity(const Entity &other) :
	mName(other.mName)
{

	//TODO copy Components

}

Entity::Entity(Entity &&other) :
	mName(other.mName),
	mComponents(std::forward<decltype(mComponents)>(other.mComponents))	
{

}

Entity::Entity(const std::string &behaviour, const std::string &name) : 
	mName(name),
	mBehaviour(behaviour)
{
}

Entity::~Entity()
{	

	mComponents.clear();

}

bool Entity::init()
{
	if (!Scope::init())
		return false;



	return true;
}


void Entity::onLoad()
{
    callMethodIfAvailable("onLoad");
}

/*std::string Entity::getName() const
{
	return mName;
}*/

const std::string & Entity::key() const
{
	return mName;
}

std::string Entity::getIdentifier() const
{
    return mName + "[" + getName() + "]";
}

void Entity::writeCreationData(Serialize::SerializeOutStream &of) const
{
	SerializableUnitBase::writeCreationData(of);
    of << mName << mName << getObjectName();
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
        throw ComponentException(Exceptions::doubleComponent(component->getName()));
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
	Engine::Scene::SceneManagerBase::getSingleton().removeLater(this);
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
	std::array<float, 3> v;
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
	std::array<float, 3> c = getCenter();
	return{ { c[0], c[2] } };
}

std::array<float, 2> Entity::getPosition2D() const {
	std::array<float, 3> p = getPosition();
	return{ { p[0], p[2] } };
}

}
}


}
