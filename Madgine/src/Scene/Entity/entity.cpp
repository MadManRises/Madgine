#include "madginelib.h"

#include "entity.h"

#include "masks.h"

#include "exceptionmessages.h"

#include "componentexception.h"

#include "Scene/ogrescenemanager.h"

#include "Serialize\Streams/serializestream.h"


#include "Scripting\Parsing\scriptparser.h"
#include "Scripting\Parsing\entitynode.h"


namespace Engine {

	API_IMPL(Scene::Entity::Entity, &addComponent, &remove, /*&enqueueMethod,*/ &getPosition, &getCenter, &setObjectVisible);


namespace Scene {
namespace Entity {

Entity *Entity::entityFromMovable(Ogre::MovableObject *o)
{
    const Ogre::Any &any =
        o->getParentSceneNode()->getUserObjectBindings().getUserAny("entity");

    return any.isEmpty() ? 0 : Ogre::any_cast<Entity *>(any);
}


Entity::Entity(Ogre::SceneNode *node, const std::string &behaviour, Ogre::Entity *obj) :
	mDescription(&Scripting::Parsing::ScriptParser::getSingleton().getEntityDescription(behaviour)),
    mNode(node),
    mObject(obj),
    mLastPosition(node->getPosition()),
	mComponents(this, &Entity::createComponent)
{

    mNode->getUserObjectBindings().setUserAny("entity", Ogre::Any(this));

	if (mObject) {
		mObject->addQueryFlags(Masks::ENTITY_MASK);
	}
	
    mDecoratorNode = mNode->createChildSceneNode(getName() + "_Decorator");

}

Entity::~Entity()
{

	for (const Ogre::unique_ptr<BaseEntityComponent> &c : mComponents) {
		c->preDelete();
	}

	clear();

	if (mObject)
		mNode->detachObject(mObject);

	mComponents.clear();

	if (mNode->getAttachedObjectIterator().hasMoreElements())
		LOG_ERROR(Exceptions::nodeNotCleared);

	mNode->getParentSceneNode()->removeChild(mNode);
	mNode->getCreator()->destroySceneNode(mNode);
}

void Entity::init(const Scripting::ArgumentList &args)
{
	if (!mDescription->getPrototype().empty())
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

std::string Entity::getName() const
{
	return mNode->getName();
}

void Entity::setObjectVisible(bool b)
{
	mObject->setVisible(b);
}

void Entity::writeCreationData(Serialize::SerializeOutStream &of) const
{
    Scope::writeCreationData(of);
    of << getName() << (mObject ? mObject->getMesh()->getName().c_str() : "") << mDescription->getName();
}

Ogre::SceneNode *Entity::getNode()
{
    return mNode;
}


Ogre::Entity *Entity::getObject()
{
    return mObject;
}

void Entity::setPosition(const Ogre::Vector3 &v)
{
    mNode->setPosition(v);
}

void Entity::translate(const Ogre::Vector3 & v)
{
	mNode->translate(v);
}

void Entity::rotate(const Ogre::Quaternion &q)
{
    mNode->rotate(q);
}

const Ogre::Vector3 &Entity::getPosition() const
{
    return mNode->getPosition();
}

Ogre::Vector3 Entity::getCenter() const
{
    return (mObject ? mObject->getWorldBoundingBox().getCenter() : mNode->getPosition());
}



Ogre::Vector2 Entity::getPosition2D() const
{
	const Ogre::Vector3 &pos = getPosition();
	return{ pos.x, pos.z };
}

Ogre::Vector2 Entity::getCenter2D() const
{
	const Ogre::Vector3 &c = getCenter();
	return{ c.x, c.z };
}

const Ogre::Quaternion & Entity::getOrientation() const
{
	return mNode->getOrientation();
}

const Ogre::Vector3 & Entity::getScale() const
{
	return mNode->getScale();
}

void Entity::positionChanged(const Ogre::Vector3 &dist)
{
    for (const Ogre::unique_ptr<BaseEntityComponent> &c : mComponents){
        c->positionChanged(dist);
    }
}

void Entity::update(float timeSinceLastFrame)
{
    if (mLastPosition != getPosition()) {
        positionChanged(getPosition() - mLastPosition);
        mLastPosition = getPosition();
    }
}

bool Entity::hasComponent(const std::string & name)
{
	return mComponents.contains(name);
}

void Entity::addComponent(const std::string &name){
	addComponentImpl(std::get<0>(createComponent(name)));
}

void Entity::removeComponent(const std::string & name)
{
	auto it = mComponents.find(name);
	assert(it != mComponents.end());
	(*it)->preDelete();
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
    for (const Ogre::unique_ptr<BaseEntityComponent> &c : mComponents){
        if(c->hasComponentMethod(name))
            return c->execComponentMethod(name, args);
    }
    return ScopeImpl::methodCall(name, args);
}

std::tuple<std::unique_ptr<BaseEntityComponent>> Entity::createComponent(const std::string & name)
{
	auto it = sRegisteredComponentsByName().find(name);
	if (it == sRegisteredComponentsByName().end())
		throw ComponentException(Exceptions::unknownComponent(name));
	return std::make_tuple((this->*(it->second))());
}

BaseEntityComponent *Entity::addComponentImpl(Ogre::unique_ptr<BaseEntityComponent> &&component)
{
    if (mComponents.find(component) != mComponents.end())
        throw ComponentException(Exceptions::doubleComponent(component->getName()));
    if (&component->getEntity() != this)
        throw ComponentException(Exceptions::corruptData);
    return mComponents.emplace(std::forward<Ogre::unique_ptr<BaseEntityComponent>>(component))->get();
}



bool Entity::hasScriptMethod(const std::string &name)
{
    return mDescription->hasMethod(name);
}

const Scripting::Parsing::MethodNode &Entity::getMethod(const std::string &name)
{
    return mDescription->getMethod(name);
}

void Entity::remove()
{
	Engine::Scene::OgreSceneManager::getSingleton().removeLater(this);
}


void Entity::writeState(Serialize::SerializeOutStream &of) const
{
	of << ValueType(getPosition());

	of << mNode->getOrientation();

	of << mNode->getScale();
	
	Scope::writeState(of);

}

void Entity::readState(Serialize::SerializeInStream &ifs)
{
	Ogre::Vector3 v;
	ifs >> v;
	setPosition(v);
	mLastPosition = v;

	Ogre::Quaternion q;
	ifs >> q;
	mNode->setOrientation(q);

	ifs >> v;
	mNode->setScale(v);

    Scope::readState(ifs);
}



Ogre::SceneNode *Entity::createDecoratorNode(bool centered, float height)
{
	Ogre::Vector3 pos;

	if (centered) {
		pos = mObject->getBoundingBox().getCenter();
	}
	else {
		pos = Ogre::Vector3::ZERO;
	}
	pos.y = height;

	return mDecoratorNode->createChildSceneNode(pos);
	
}

void Entity::destroyDecoratorNode(Ogre::SceneNode * node)
{
	if (node->getAttachedObjectIterator().begin() !=
		node->getAttachedObjectIterator().end())
		LOG_ERROR(Exceptions::nodeNotCleared);
	node->getParentSceneNode()->removeChild(node);
	node->getCreator()->destroySceneNode(node);
}

}
}


}
