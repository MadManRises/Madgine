#include "madginelib.h"
#include "baseentitycomponent.h"
#include "entity.h"

namespace Engine {
namespace Scene {
namespace Entity {

BaseEntityComponent::BaseEntityComponent(Entity &entity) :
    mEntity(entity),
	SerializableUnit(entity.topLevel())
{
}

BaseEntityComponent::~BaseEntityComponent() {

}

void BaseEntityComponent::preDelete() {

}

Entity &BaseEntityComponent::getEntity() const
{
    return mEntity;
}


void BaseEntityComponent::positionChanged(const Ogre::Vector3 &){}

Ogre::Vector2 BaseEntityComponent::getPosition2D()
{
    return mEntity.getPosition2D();
}

const Ogre::Vector3 &BaseEntityComponent::getPosition()
{
    return mEntity.getPosition();
}

Ogre::Vector2 BaseEntityComponent::getCenter2D()
{
	return mEntity.getCenter2D();
}

Ogre::Vector3 BaseEntityComponent::getCenter()
{
	return mEntity.getCenter();
}

const Ogre::Quaternion & BaseEntityComponent::getOrientation()
{
	return mEntity.getOrientation();
}

const Ogre::Vector3 & BaseEntityComponent::getScale()
{
	return mEntity.getScale();
}

}
}
}
