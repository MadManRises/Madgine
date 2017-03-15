#pragma once

#include "Serialize\ogreSerialize.h"

#include "Serialize\Container\set.h"

#include "Scene\Entity\entity.h"


namespace Engine {
namespace Scene {
namespace Entity {




class OGREMADGINE_EXPORT OgreEntity : public Entity
{

public:
    static OgreEntity *entityFromMovable(Ogre::MovableObject *o);

	OgreEntity(const OgreEntity&);
	OgreEntity(OgreEntity &&);

    OgreEntity(const Scripting::Parsing::EntityNode *behaviour, Ogre::SceneNode *node, Ogre::Entity *obj);
    ~OgreEntity();

	Ogre::SceneNode *createDecoratorNode(bool centered, float height = 0.0f);
	void destroyDecoratorNode(Ogre::SceneNode* node);

	Ogre::SceneNode *getNode();
	Ogre::Entity *getObject();
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
    
	Ogre::SceneNode *mNode;
    Ogre::SceneNode *mDecoratorNode;
	Ogre::Entity *mObject;
    
    //std::set<Ogre::unique_ptr<EntityComponentBase>> mComponents;

};

}
}
}
