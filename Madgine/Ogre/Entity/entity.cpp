#include "libinclude.h"

#include "entity.h"

#include "masks.h"

#include "Database\exceptionmessages.h"

#include "componentexception.h"

#include "OGRE\scenemanager.h"

#include "Scripting\Datatypes\Serialize\serializestream.h"

#include "Ogre\entityexception.h"

#include "Scripting\Parsing\scriptparser.h"
#include "Scripting\Parsing\entitynode.h"



namespace Engine {

	API_IMPL(OGRE::Entity::Entity, &addComponent, &remove, /*&enqueueMethod,*/ &getPosition, &getCenter, &setObjectVisible);



OGRE::Entity::Entity::Factory OGRE::Entity::Entity::sFactory;

namespace OGRE {
namespace Entity {

Entity *Entity::entityFromMovable(Ogre::MovableObject *o)
{
    const Ogre::Any &any =
        o->getParentSceneNode()->getUserObjectBindings().getUserAny("entity");

    return any.isEmpty() ? 0 : Ogre::any_cast<Entity *>(any);
}


Entity::Entity(Ogre::SceneNode *node, const std::string &behaviour, Ogre::Entity *obj) :
	mDescription(Scripting::Parsing::ScriptParser::getSingleton().getEntityDescription(behaviour)),
    mNode(node),
    mObject(obj),
    mLastPosition(node->getPosition())
{

	setData(mDescription->getPrototype());

    mNode->getUserObjectBindings().setUserAny("entity", Ogre::Any(this));

	if (mObject) {
		mObject->addQueryFlags(Masks::ENTITY_MASK);
	}
	
    mDecoratorNode = mNode->createChildSceneNode(getName() + "_Decorator");

}

Entity::~Entity()
{
}

void Entity::init(const Scripting::ArgumentList &args)
{
    callMethodIfAvailable("init", args);
}

void Entity::finalize()
{
	clear();

	if (mObject)
		mNode->detachObject(mObject);

	for (std::pair<const std::string, Ogre::unique_ptr<BaseEntityComponent>> &p : mComponents) {
		p.second->finalize();
	}

	mComponents.clear();

}



void Entity::onLoad()
{
    callMethodIfAvailable("onLoad");
}



std::string Entity::getIdentifier()
{
    return mDescription->getName() + "[" + getName() + "]";
}

std::string Entity::getName()
{
	return mNode->getName();
}

void Entity::setObjectVisible(bool b)
{
	mObject->setVisible(b);
}

void Entity::storeCreationData(Scripting::Serialize::SerializeOutStream &of)
{
    Scope::storeCreationData(of);
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

void Entity::positionChanged(const Ogre::Vector3 &dist)
{
    for (std::pair<const std::string, Ogre::unique_ptr<BaseEntityComponent>> &p : mComponents){
        p.second->positionChanged(dist);
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
	return mComponents.find(name) != mComponents.end();
}

BaseEntityComponent *Entity::addComponent(const std::string &name){
	auto it = sRegisteredComponentsByName().find(name);
	if (it == sRegisteredComponentsByName().end())
		throw ComponentException(Database::Exceptions::unknownComponent(name));
	return (this->*(it->second))();
}

void Entity::removeComponent(const std::string & name)
{
	assert(hasComponent(name));
	mComponents.erase(name);
}

bool Entity::existsComponent(const std::string &name)
{
    return sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end();
}

std::set<std::string> Entity::registeredComponentNames()
{
	std::set<std::string> result;

	for (const std::pair<const std::string, ComponentAdder> &p : sRegisteredComponentsByName()) {
		result.insert(p.first);
	}

	return result;
}

Scripting::ValueType Entity::methodCall(const std::string &name, const Scripting::ArgumentList &args)
{
    for (std::pair<const std::string, Ogre::unique_ptr<BaseEntityComponent>> &p : mComponents){
        if(p.second->hasComponentMethod(name))
            return p.second->execComponentMethod(name, args);
    }
    return ScopeImpl::methodCall(name, args);
}

void Entity::addComponentImpl(const std::string &name, Ogre::unique_ptr<BaseEntityComponent> &&component)
{
    if (mComponents.find(name) != mComponents.end())
        throw ComponentException(Database::Exceptions::doubleComponent(name));
    if (&component->getEntity() != this)
        throw ComponentException(Database::Exceptions::corruptData);
    mComponents.emplace(name, std::forward<Ogre::unique_ptr<BaseEntityComponent>>(component));
}



bool Entity::hasScriptMethod(const std::string &name)
{
    return mDescription->hasMethod(name);
}

const Scripting::Parsing::MethodNodePtr &Entity::getMethod(const std::string &name)
{
    return mDescription->getMethod(name);
}

void Entity::remove()
{
	Engine::OGRE::SceneManager::getSingleton().removeLater(this);
}




/*
ValueType Entity::enqueueMethod(const ArgumentList &stack)
{
    mEnqueuedMethods.emplace_back(stack.at(0).asFloat(), stack.at(1).asString(),
		ArgumentList(stack.begin()+2, stack.end()));

    return ValueType();
}*/


void Entity::save(Scripting::Serialize::SerializeOutStream &of) const
{
    Scope::save(of);

    of << getPosition();

	of << mNode->getOrientation();

	of << mNode->getScale();

    for (const std::pair<const std::string, Ogre::unique_ptr<BaseEntityComponent>> &comp : mComponents){
        of << comp.first << *comp.second;
    }

    of << Scripting::ValueType();
}

void Entity::load(Scripting::Serialize::SerializeInStream &ifs)
{
    Scope::load(ifs);

    Ogre::Vector3 v;
    ifs >> v;
    setPosition(v);
    mLastPosition = v;

	Ogre::Quaternion q;
	ifs >> q;
	mNode->setOrientation(q);

	ifs >> v;
	mNode->setScale(v);



    std::string componentName;
    while(ifs.loopRead(componentName)){
        ifs >> *addComponent(componentName);
    }

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
		throw EntityException(Database::Exceptions::nodeNotCleared);
	node->getParentSceneNode()->removeChild(node);
	node->getCreator()->destroySceneNode(node);
}

}
}

Scripting::Scope *OGRE::Entity::Entity::Factory::create(Scripting::Serialize::SerializeInStream &in)
{
	std::string obName, entityMesh, behaviour;
	in >> obName >> entityMesh >> behaviour;
	return OGRE::SceneManager::getSingleton().createEntity(obName, entityMesh, behaviour);
}

}
