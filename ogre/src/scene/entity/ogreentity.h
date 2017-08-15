#pragma once

#include "scene/entity/entity.h"


namespace Engine {
namespace Scene {
namespace Entity {

class OGREMADGINE_EXPORT OgreEntity : public Entity
{

public:
    static OgreEntity *entityFromMovable(Ogre::MovableObject *o);

	OgreEntity(const OgreEntity&);
	OgreEntity(OgreEntity &&);

    OgreEntity(SceneManagerBase *sceneMgr, Ogre::SceneNode *node, Ogre::Entity *obj, const std::string &behaviour = "");
    ~OgreEntity();

	Ogre::SceneNode *createDecoratorNode(bool centered, float height = 0.0f);
	void destroyDecoratorNode(Ogre::SceneNode* node);

	Ogre::SceneNode *getNode();
	Ogre::Entity *getObject();
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
    
	Ogre::SceneNode *mNode;
    Ogre::SceneNode *mDecoratorNode;
	Ogre::Entity *mObject;
    

};

}
}
}
