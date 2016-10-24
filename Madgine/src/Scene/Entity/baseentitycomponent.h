#pragma once

#include "Scripting/Datatypes/Serialize/serializable.h"
#include "Scripting\Datatypes\argumentlist.h"
#include "Util\UtilMethods.h"

namespace Engine {
namespace Scene {
namespace Entity {

class MADGINE_EXPORT BaseEntityComponent : public Ogre::SceneObjAllocatedObject, public Scripting::Serialize::Serializable {
public:
    BaseEntityComponent(Entity &entity);
    virtual ~BaseEntityComponent() = 0;

	virtual void finalize();

    virtual void positionChanged(const Ogre::Vector3&);
    virtual std::string name() const = 0;

    virtual bool hasComponentMethod(const std::string &name) const = 0;
    virtual Scripting::ValueType execComponentMethod(const std::string &name, const Scripting::ArgumentList &args) = 0;

    Ogre::Vector2 getPosition2D();
    const Ogre::Vector3 &getPosition();

    Entity &getEntity() const;

protected:
    virtual void save(Scripting::Serialize::SerializeOutStream &out) const override;
    virtual void load(Scripting::Serialize::SerializeInStream &in) override;

    Entity &mEntity;
};

}
}
}


