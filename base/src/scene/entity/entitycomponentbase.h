 #pragma once

#include "serialize/serializableunit.h"
#include "scripting/types/scopebase.h"

namespace Engine {
namespace Scene {
namespace Entity {

class MADGINE_BASE_EXPORT EntityComponentBase : public Serialize::SerializableUnitBase, public Scripting::ScopeBase {
public:
    EntityComponentBase(Entity &entity, const Scripting::LuaTable &initTable);
    virtual ~EntityComponentBase() = 0;

	virtual bool init() override;


    std::array<float, 2> getPosition2D();
	void setPosition(const Vector3 &pos);
	Vector3 getPosition();
	std::array<float, 2> getCenter2D();
	Vector3 getCenter();
	std::array<float, 4> getOrientation();
	void setScale(const Vector3 &scale);
	Vector3 getScale();

    Entity &getEntity() const;

protected:

    Entity &mEntity;
	Scripting::LuaTable mInitTable;
};

}
}
}


