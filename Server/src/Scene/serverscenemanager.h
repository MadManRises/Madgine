#pragma once

#include "Serialize\Container\list.h"

#include "Scene\scenemanager.h"

#include "Scene\Entity\masks.h"

#include "Entity\serverentity.h"

namespace Engine {
namespace Scene {

class MADGINE_SERVER_EXPORT ServerSceneManager :
	public SceneManager
{
public:
    ServerSceneManager();
    virtual ~ServerSceneManager();

	//using Ogre::Singleton<OgreSceneManager>::getSingleton;

    virtual bool init() override;
	virtual void finalize() override;

	std::tuple<const Scripting::Parsing::EntityNode *, std::string, std::string> createEntityData(const std::string &behaviour, const std::string &name, const std::string &meshName);
	
	Entity::Entity *createEntity(const std::string &behaviour = "", const std::string &name = "", const std::string &meshName = "", const Scripting::ArgumentList &args = {});
	std::list<Entity::Entity *> entities();
	Entity::Entity *findEntity(const std::string &name);

	void clear();

	virtual void writeState(Serialize::SerializeOutStream &out) const override;
	virtual void readState(Serialize::SerializeInStream &in) override;

	void readScene(Serialize::SerializeInStream &in, bool callInit = false);
	void writeScene(Serialize::SerializeOutStream &out) const;

	static constexpr const float sSceneRasterSize = .2f;
	
	void makeLocalCopy(Entity::ServerEntity &e);
	void makeLocalCopy(Entity::ServerEntity &&e);

	void setEntitiesCallback(std::function<void(const Serialize::SerializableList < Entity::ServerEntity, const Scripting::Parsing::EntityNode *, const std::string &, const std::string & > ::iterator &, int) > f);

protected:

	void removeQueuedEntities();
		
    
private:

	std::string mStaticSceneName;
	

	Serialize::ObservableList<Entity::ServerEntity, Serialize::ContainerPolicy::masterOnly, const Scripting::Parsing::EntityNode *, const std::string &, const std::string &> mEntities;
	std::list<Entity::ServerEntity> mLocalEntities;
    
};

}
}


