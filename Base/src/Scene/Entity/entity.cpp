#include "baselib.h"

#include "entity.h"

#include "masks.h"

#include "componentexception.h"

#include "Scene/scenemanager.h"

#include "Serialize\Streams/serializestream.h"


#include "Scripting\Parsing\scriptparser.h"
#include "Scripting\Parsing\entitynode.h"


namespace Engine {

	API_IMPL(Scene::Entity::Entity, &addComponent, &remove, /*&enqueueMethod,*/ &getPosition, &getCenter, &setObjectVisible);


namespace Scene {
namespace Entity {



Entity::Entity(const Entity &other) :
	Scope(other),
	mDescription(other.mDescription),
	mComponents()	
{

	//TODO copy Components

}

Entity::Entity(Entity &&other) :
	Scope(std::forward<Entity>(other)),
	mDescription(other.mDescription),
	mComponents(std::forward<decltype(mComponents)>(other.mComponents))	
{
}

Entity::Entity(const Scripting::Parsing::EntityNode *behaviour) :
	mDescription(behaviour),
	mComponents()
{
}

Entity::~Entity()
{	

	clear();

	mComponents.clear();

}

void Entity::init(const Scripting::ArgumentList &args)
{
	if (mDescription && !mDescription->getPrototype().empty())
		findPrototype(mDescription->getPrototype());

    callMethodIfAvailable("init", args);
}


void Entity::onLoad()
{
    callMethodIfAvailable("onLoad");
}



std::string Entity::getIdentifier()
{
    return mDescription->getName() + "[" + getName() + "]";
}

void Entity::writeCreationData(Serialize::SerializeOutStream &of) const
{
    Scope::writeCreationData(of);
    of << mDescription->getName() << getName() << getObjectName();
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

void Entity::addComponent(const Scripting::ArgumentList &args, const std::string &name){
	addComponentImpl(std::get<0>(createComponent(name, args)));
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

ValueType Entity::methodCall(const std::string &name, const Scripting::ArgumentList &args)
{
    for (const std::unique_ptr<EntityComponentBase> &c : mComponents){
        if(c->hasComponentMethod(name))
            return c->execComponentMethod(name, args);
    }
    return Scope::methodCall(name, args);
}

std::tuple<std::unique_ptr<EntityComponentBase>> Entity::createComponent(const std::string & name, const Engine::Scripting::ArgumentList &args)
{
	auto it = sRegisteredComponentsByName().find(name);
	if (it == sRegisteredComponentsByName().end())
		throw ComponentException(Exceptions::unknownComponent(name));
	return std::make_tuple(it->second(*this, args));
}

EntityComponentBase *Entity::addComponentImpl(std::unique_ptr<EntityComponentBase> &&component)
{
    if (mComponents.find(component) != mComponents.end())
        throw ComponentException(Exceptions::doubleComponent(component->getName()));
    if (&component->getEntity() != this)
        throw ComponentException(Exceptions::corruptData);
    return mComponents.emplace(std::forward<std::unique_ptr<EntityComponentBase>>(component))->get();
}



bool Entity::hasScriptMethod(const std::string &name)
{
	if (!mDescription) return false;
    return mDescription->hasMethod(name);
}

const Scripting::Parsing::MethodNode &Entity::getMethod(const std::string &name)
{
	if (!mDescription) throw 0;
    return mDescription->getMethod(name);
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
	
	Scope::writeState(of);

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

    Scope::readState(ifs);
}

std::array<float, 2> Entity::getCenter2D() const {
	std::array<float, 3> c = getCenter();
	return{ c[0], c[2] };
}

std::array<float, 2> Entity::getPosition2D() const {
	std::array<float, 3> p = getPosition();
	return{ p[0], p[2] };
}

}
}


}
