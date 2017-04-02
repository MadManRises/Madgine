#pragma once

#include "Serialize/serializableunit.h"
#include "Util\UtilMethods.h"
#include "Serialize\Container\set.h"

namespace Engine {
namespace Scene {
namespace Entity {

class MADGINE_BASE_EXPORT EntityComponentBase : public Serialize::SerializableUnitBase, public Serialize::Keyed<std::string> {
public:
    EntityComponentBase(Entity &entity);
    virtual ~EntityComponentBase() = 0;

    virtual std::string getName() const = 0;

    virtual bool hasComponentMethod(const std::string &name) const = 0;
    virtual ValueType execComponentMethod(const std::string &name, const Scripting::ArgumentList &args) = 0;

    std::array<float, 2> getPosition2D();
	void setPosition(const std::array<float, 3> &pos);
    std::array<float, 3> getPosition();
	std::array<float, 2> getCenter2D();
	std::array<float, 3> getCenter();
	std::array<float, 4> getOrientation();
	void setScale(const std::array<float, 3> &scale);
	std::array<float, 3> getScale();

    Entity &getEntity() const;

	virtual Scripting::ArgumentList creationArguments() const;

protected:

    Entity &mEntity;
};

}
}
}


