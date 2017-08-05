#pragma once

#include "serialize/container/list.h"

#include "scene/scenemanager.h"

#include "scene/entity/masks.h"

#include "entity/serverentity.h"

#include "scene/light.h"

namespace Engine {
namespace Scene {

class MADGINE_SERVER_EXPORT ServerSceneManager :
	public SceneManager<ServerSceneManager>
{
public:
    ServerSceneManager();
    virtual ~ServerSceneManager();	

    virtual bool init() override;
	virtual void finalize() override;

	std::tuple<ServerSceneManager*, std::string, std::string> createEntityData(const std::string &name, const std::string &meshName);
	
	virtual Entity::Entity *createEntity(const std::string &behaviour = "", const std::string &name = "", const std::string &meshName = "", std::function<void(Entity::Entity&)> init = {}) override;
	virtual Entity::Entity *createLocalEntity(const std::string &behaviour = "", const std::string &name = "", const std::string &meshName = "") override;
	virtual std::list<Entity::Entity *> entities() override;
	virtual Entity::Entity *findEntity(const std::string &name) override;
	
	virtual Light * createLight() override;
	virtual std::list<Light*> lights() override;

	virtual void clear() override;

	virtual void writeState(Serialize::SerializeOutStream &out) const override;
	virtual void readState(Serialize::SerializeInStream &in) override;

	void readScene(Serialize::SerializeInStream &in);
	void writeScene(Serialize::SerializeOutStream &out) const;

	static constexpr const float sSceneRasterSize = .2f;
	
	void makeLocalCopy(Entity::ServerEntity &e);
	void makeLocalCopy(Entity::ServerEntity &&e);

	template <class T>
	void connectEntitiesCallback(T &slot) {
		mEntities.connectCallback(slot);
	}

	virtual Scripting::KeyValueMapList maps() override;

protected:

	virtual void removeQueuedEntities() override;
		
    
private:

	std::string mStaticSceneName;
	

	Serialize::ObservableList<Entity::ServerEntity, Serialize::ContainerPolicy::masterOnly, Serialize::ParentCreator<decltype(&ServerSceneManager::createEntityData), &ServerSceneManager::createEntityData>> mEntities;
	std::list<Entity::ServerEntity> mLocalEntities;

	Serialize::ObservableList<Light, Serialize::ContainerPolicy::masterOnly> mLights;

};

}
}


