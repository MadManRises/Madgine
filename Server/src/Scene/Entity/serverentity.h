#pragma once

#include "Scripting/Types/scope.h"

#include "Serialize\Container\set.h"

#include "Scene\Entity\entity.h"


namespace Engine {
namespace Scene {
namespace Entity {




class MADGINE_SERVER_EXPORT ServerEntity : public Entity
{

public:
	ServerEntity(const ServerEntity&) = default;
	ServerEntity(ServerEntity &&) = default;

	ServerEntity(const Scripting::Parsing::EntityNode *behaviour, const std::string &name, const std::string &object);
    ~ServerEntity();

	virtual std::array<float, 3> getPosition() const override;
	virtual std::array<float, 3> getCenter() const override;
	virtual std::array<float, 4> getOrientation() const override;
	virtual std::array<float, 3> getScale() const override;

	virtual std::string getName() const override;
	virtual std::string getObjectName() const override;

	virtual void setObjectVisible(bool b) override;

	virtual void setPosition(const std::array<float, 3> &v) override;
	virtual void setScale(const std::array<float, 3> &scale) override;
	virtual void setOrientation(const std::array<float, 4> &orientation) override;
	virtual void translate(const std::array<float, 3> &v) override;
	virtual void rotate(const std::array<float, 4> &q) override;

private:
    
	std::array<float, 3> mPosition;
	std::array<float, 3> mScale;
	std::array<float, 4> mOrientation;

	std::string mName;
	std::string mObject;



    //std::set<Ogre::unique_ptr<EntityComponentBase>> mComponents;

};

}
}
}
