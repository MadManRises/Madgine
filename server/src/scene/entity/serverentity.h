#pragma once

#include "scene/entity/entity.h"


namespace Engine {
namespace Scene {
namespace Entity {




class MADGINE_SERVER_EXPORT ServerEntity : public Entity
{

public:
	ServerEntity(const ServerEntity&) = default;
	ServerEntity(ServerEntity &&) = default;

	ServerEntity(ServerSceneManager *sceneMgr, const std::string &name, const std::string &object, const std::string &behaviour = "");
    ~ServerEntity();

	virtual Vector3 getPosition() const override;
	virtual Vector3 getCenter() const override;
	virtual std::array<float, 4> getOrientation() const override;
	virtual Vector3 getScale() const override;

	virtual std::string getObjectName() const override;

	virtual void setObjectVisible(bool b) override;

	virtual void setPosition(const Vector3 &v) override;
	virtual void setScale(const Vector3 &scale) override;
	virtual void setOrientation(const std::array<float, 4> &orientation) override;
	virtual void translate(const Vector3 &v) override;
	virtual void rotate(const std::array<float, 4> &q) override;

private:
    
	Vector3 mPosition;
	Vector3 mScale;
	std::array<float, 4> mOrientation;

	std::string mObject;

};

}
}
}
