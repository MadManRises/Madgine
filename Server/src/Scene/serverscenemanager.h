#pragma once

#include "Serialize\Container\list.h"

#include "Scene\scenemanager.h"

#include "Scene\Entity\masks.h"

#include "Entity\serverentity.h"

#include "Scene\light.h"

namespace Engine {
namespace Scene {

class MADGINE_SERVER_EXPORT ServerSceneManager :
	public SceneManager
{
public:
    ServerSceneManager();
    virtual ~ServerSceneManager();	

    virtual bool init() override;
	virtual void finalize() override;

	std::tuple<const Scripting::Parsing::EntityNode *, std::string, std::string> createEntityData(const std::string &behaviour, const std::string &name, const std::string &meshName);
	
	virtual Entity::Entity *createEntity(const std::string &behaviour = "", const std::string &name = "", const std::string &meshName = "", const Scripting::ArgumentList &args = {}, std::function<void(Entity::Entity&)> init = {}) override;
	virtual Entity::Entity *createLocalEntity(const std::string &behaviour = "", const std::string &name = "", const std::string &meshName = "", const Scripting::ArgumentList &args = {}) override;
	virtual std::list<Entity::Entity *> entities() override;
	virtual Entity::Entity *findEntity(const std::string &name) override;
	
	virtual Light * createLight() override;
	virtual std::list<Light*> lights() override;

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

	virtual void removeQueuedEntities() override;
		
    
private:

	std::string mStaticSceneName;
	

	Serialize::ObservableList<Entity::ServerEntity, Serialize::ContainerPolicy::masterOnly, const Scripting::Parsing::EntityNode *, const std::string &, const std::string &> mEntities;
	std::list<Entity::ServerEntity> mLocalEntities;

	Serialize::ObservableList<Light, Serialize::ContainerPolicy::masterOnly> mLights;

};

}
}


