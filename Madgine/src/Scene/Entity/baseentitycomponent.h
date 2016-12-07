#pragma once

#include "Serialize/serializableunit.h"
#include "Util\UtilMethods.h"
#include "Serialize\Container\set.h"

namespace Engine {
namespace Scene {
namespace Entity {

class MADGINE_EXPORT BaseEntityComponent : public Ogre::SceneObjAllocatedObject, public Serialize::SerializableUnit, public Serialize::Keyed<std::string> {
public:
    BaseEntityComponent(Entity &entity);
    virtual ~BaseEntityComponent() = 0;

	virtual void preDelete();

    virtual void positionChanged(const Ogre::Vector3&);
    virtual std::string getName() const = 0;

    virtual bool hasComponentMethod(const std::string &name) const = 0;
    virtual ValueType execComponentMethod(const std::string &name, const Scripting::ArgumentList &args) = 0;

    Ogre::Vector2 getPosition2D();
    const Ogre::Vector3 &getPosition();
	Ogre::Vector2 getCenter2D();
	Ogre::Vector3 getCenter();
	const Ogre::Quaternion &getOrientation();
	const Ogre::Vector3 &getScale();

    Entity &getEntity() const;

protected:

    Entity &mEntity;
};

}
}
}


