#include "serverlib.h"

#include "serverentity.h"


#include "Scene/serverscenemanager.h"

#include "Serialize\Streams/serializestream.h"


#include "Scripting\Parsing\scriptparser.h"
#include "Scripting\Parsing\entitynode.h"

#include "Scene\Entity\masks.h"

namespace Engine {

	

namespace Scene {
namespace Entity {



	ServerEntity::ServerEntity(const Scripting::Parsing::EntityNode *behaviour, const std::string &name, const std::string &object) :
	Entity(behaviour),
    mName(name),
    mObject(object),
		mOrientation{1, 0, 0, 0},
		mScale{1, 1, 1}
{

}

	ServerEntity::~ServerEntity()
{
}



std::string ServerEntity::getName() const
{
	return mName;
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
