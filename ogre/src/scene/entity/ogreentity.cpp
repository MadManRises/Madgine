#include "madginelib.h"

#include "ogreentity.h"


#include "Scene/ogrescenemanager.h"

#include "Serialize\Streams/serializestream.h"


#include "Scripting\Parsing\scriptparser.h"

#include "Scene\Entity\masks.h"

namespace Engine {

	

namespace Scene {
namespace Entity {

OgreEntity *OgreEntity::entityFromMovable(Ogre::MovableObject *o)
{
    const Ogre::Any &any =
        o->getParentSceneNode()->getUserObjectBindings().getUserAny("entity");

    return any.isEmpty() ? 0 : Ogre::any_cast<OgreEntity *>(any);
}


OgreEntity::OgreEntity(const OgreEntity &other) :
	Entity(other)	
{

	mNode = other.mNode->getParentSceneNode()->createChildSceneNode(Ogre::Vector3(other.getPosition().ptr()), Ogre::Quaternion(const_cast<float*>(other.getOrientation().data())));
	mNode->getUserObjectBindings().setUserAny("entity", Ogre::Any(this));

	if (other.mObject) {
		//TODO
	}

	//TODO copy Components

	mDecoratorNode = mNode->createChildSceneNode(getName() + "_Decorator");

}

OgreEntity::OgreEntity(OgreEntity &&other) :
	Entity(std::forward<Entity>(other)),
	mNode(other.mNode),
	mDecoratorNode(other.mDecoratorNode),
	mObject(other.mObject)
{
	other.mNode = 0;
	other.mObject = 0;
	other.mDecoratorNode = 0;

	//Change components-Owner
}

OgreEntity::OgreEntity(const std::string &behaviour, Ogre::SceneNode *node, Ogre::Entity *obj) :
	Entity(behaviour, node->getName().c_str()),
    mNode(node),
    mObject(obj)
{

    mNode->getUserObjectBindings().setUserAny("entity", Ogre::Any(this));

	if (mObject) {
		mObject->addQueryFlags(Masks::ENTITY_MASK);
	}
	
    mDecoratorNode = mNode->createChildSceneNode(node->getName() + "_Decorator");

	init();

}

OgreEntity::~OgreEntity()
{

	if (mObject)
		mNode->detachObject(mObject);

	if (mNode) {
		if (mNode->getAttachedObjectIterator().hasMoreElements())
			LOG_ERROR(Exceptions::nodeNotCleared);

		mNode->getParentSceneNode()->removeChild(mNode);
		mNode->getCreator()->destroySceneNode(mNode);
	}
}

std::string OgreEntity::getObjectName() const
{
	return mObject ? mObject->getName() : "";
}

void OgreEntity::setObjectVisible(bool b)
{
	mObject->setVisible(b);
}

Ogre::SceneNode *OgreEntity::getNode()
{
    return mNode;
}


Ogre::Entity *OgreEntity::getObject()
{
    return mObject;
}

void OgreEntity::setPosition(const Vector3 &v)
{
    mNode->setPosition(Ogre::Vector3(v.ptr()));
}

void OgreEntity::setScale(const Vector3 & scale)
{
	mNode->setScale(Ogre::Vector3(scale.ptr()));
}

void OgreEntity::setOrientation(const std::array<float, 4>& orientation)
{
	mNode->setOrientation(Ogre::Quaternion(const_cast<float*>(orientation.data())));
}

void OgreEntity::translate(const Vector3 & v)
{
	mNode->translate(Ogre::Vector3(v.ptr()));
}

void OgreEntity::rotate(const std::array<float, 4> &q)
{
    mNode->rotate(Ogre::Quaternion(const_cast<float*>(q.data())));
}

Vector3 OgreEntity::getPosition() const
{
	return{ mNode->getPosition().x, mNode->getPosition().y, mNode->getPosition().z };
}

Vector3 OgreEntity::getCenter() const
{
	Ogre::Vector3 v = (mObject ? mObject->getWorldBoundingBox().getCenter() : mNode->getPosition());
	return{ v.x, v.y, v.z };
}


std::array<float, 4> OgreEntity::getOrientation() const
{
	return{ {mNode->getOrientation().w, mNode->getOrientation().x, mNode->getOrientation().y, mNode->getOrientation().z} };
}

Vector3 OgreEntity::getScale() const
{
	return{ mNode->getScale().x, mNode->getScale().y, mNode->getScale().z };
}


Ogre::SceneNode *OgreEntity::createDecoratorNode(bool centered, float height)
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

void OgreEntity::destroyDecoratorNode(Ogre::SceneNode * node)
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
