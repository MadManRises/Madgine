#include "serverlib.h"

#include "serverentity.h"


#include "scene/serverscenemanager.h"

#include "serialize/streams/serializestream.h"


#include "scripting/parsing/scriptparser.h"

#include "scene/entity/masks.h"

namespace Engine {

	

namespace Scene {
namespace Entity {



	ServerEntity::ServerEntity(const std::string &behaviour, const std::string &name, const std::string &object) :
	Entity(behaviour, name),
		mScale{ { 1, 1, 1 } },
		mOrientation{ {1, 0, 0, 0} },
		mObject(object)
{

}

	ServerEntity::~ServerEntity()
{
}

std::string ServerEntity::getObjectName() const
{
	return mObject;
}

void ServerEntity::setObjectVisible(bool b)
{
	//mObject->setVisible(b);
}

void ServerEntity::setPosition(const std::array<float, 3> &v)
{
	mPosition = v;
}

void ServerEntity::setScale(const std::array<float, 3> & scale)
{
	mScale = scale;
}

void ServerEntity::setOrientation(const std::array<float, 4>& orientation)
{
	mOrientation = orientation;
}

void ServerEntity::translate(const std::array<float, 3> & v)
{
//	mNode->translate(Ogre::Vector3(v.data()));
}

void ServerEntity::rotate(const std::array<float, 4> &q)
{
  //  mNode->rotate(Ogre::Quaternion(const_cast<float*>(q.data())));
}

std::array<float, 3> ServerEntity::getPosition() const
{
	return mPosition;
}

std::array<float, 3> ServerEntity::getCenter() const
{
	return mPosition;
}


std::array<float, 4> ServerEntity::getOrientation() const
{
	return mOrientation;
}

std::array<float, 3> ServerEntity::getScale() const
{
	return mScale;
}

}
}


}
