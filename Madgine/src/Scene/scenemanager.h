#pragma once

#include "Scene\scenecomponent.h"
#include "Resources/Shading/shadercollector.h"
#include "Scripting\Types\refscopetoplevelserializableunit.h"
#include "Entity\masks.h"
#include "Util\MadgineObject.h"

#include "Entity\entity.h"

#include "Serialize\Container\list.h"


namespace Engine {
namespace Scene {

class MADGINE_EXPORT SceneManager : public Ogre::Singleton<SceneManager>,
	public Ogre::GeneralAllocatedObject, 
	public Scripting::RefScopeTopLevelSerializableUnit, 
	public Util::MadgineObject<SceneManager>, 
	public Scripting::GlobalAPI<SceneManager>
{
public:
    SceneManager(Ogre::Root *root);
    virtual ~SceneManager();

    virtual void init() override;
	virtual void finalize() override;

	void update(float timeSinceLastFrame, App::ContextMask mask);


    void addSceneListener(SceneListener *l);
    void removeSceneListener(SceneListener *l);


	size_t getComponentCount();

	
    void onLoad();


	void setGameTextureSize(const Ogre::Vector2 &size);
	Ogre::TexturePtr getGameTexture();
	Ogre::Camera *camera();
	Ogre::SceneManager *getSceneManager();
	Ogre::Image getScreenshot();
	Ogre::Viewport *getViewport();
	Ogre::RenderTarget *getRenderTarget();

	bool isUsingHeightmap() const;
	const std::vector<Ogre::SceneNode*> &terrainEntities();
	const Ogre::SceneNode *terrain();
	Ogre::TerrainGroup *terrainGroup() const;

	std::tuple<Ogre::SceneNode *, std::string, Ogre::Entity*> createEntityData(const std::string &name, const std::string &meshName, const std::string &behaviour);
	
	Entity::Entity *createEntity(const std::string &name, const std::string &meshName, const std::string &behaviour, const Scripting::ArgumentList &args = {});
	std::list<Entity::Entity *> entities();
	Entity::Entity *findEntity(const std::string &name);
	void removeLater(Entity::Entity *e);



	Math::Bounds getSceneBounds();
	Math::Bounds getRasterizedSceneBounds();

	Ogre::Vector3 rasterizePoint(const Ogre::Vector3 &v);
	Ogre::Vector2 rasterizePoint(const Ogre::Vector2 &v);

	Ogre::Vector2 relToScenePos(const Ogre::Vector2 &v);


	const std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion> &getInfoObject(const std::string &name) const;
	
	void GetMeshInformation(const Ogre::MeshPtr mesh, size_t &vertex_count,
		Ogre::Vector3 *&vertices, size_t &index_count, unsigned long *&indices,
		const Ogre::Vector3 &position, const Ogre::Quaternion &orient,
		const Ogre::Vector3 &scale);
	void getTerrainMeshInformation(size_t & vertex_count, Ogre::Vector3 *& vertices, size_t & index_count, unsigned int *& indices);


	Ogre::Ray mousePointToRay(const Ogre::Vector2 &mousePos);
	bool mousePointToTerrainPoint(const Ogre::Vector2 &mousePos,
		Ogre::Vector3 &result, Ogre::uint32 mask = Entity::Masks::TERRAIN_MASK);
	bool rayToTerrainPoint(const Ogre::Ray &ray, Ogre::Vector3 &result, Ogre::uint32 mask = Entity::Masks::TERRAIN_MASK);


	void saveComponentData(Serialize::SerializeOutStream &out) const;
	void loadComponentData(Serialize::SerializeInStream &in);
	

	void clear();

	virtual void writeState(Serialize::SerializeOutStream &out) const override;
	virtual void readState(Serialize::SerializeInStream &in) override;

	void readScene(Serialize::SerializeInStream &in, bool callInit = false);
	void writeScene(Serialize::SerializeOutStream &out) const;

	static constexpr const float sSceneRasterSize = .2f;

	Scripting::Scope *createSceneArray(size_t size);
	Scripting::Scope *createSceneStruct();
	Scripting::Scope *createSceneList();

	///FOR LAUNCHER
	std::set<BaseSceneComponent*> getComponents();
	std::set<SceneListener*> getListeners();


protected:

	virtual void createCamera(void);

	void writeStaticScene(Serialize::SerializeOutStream &out) const;
	void readStaticScene(Serialize::SerializeInStream &in);
	void readTerrain(Serialize::SerializeInStream &in);


	void removeQueuedEntities();
		
    bool RaycastFromPoint(const Ogre::Ray &ray, Ogre::Vector3 &result, Ogre::uint32 mask);

private:

    Ogre::Root *mRoot;

	Ogre::SceneManager *mSceneMgr;
	Ogre::TerrainGlobalOptions *mTerrainGlobals;
	Ogre::TerrainGroup *mTerrainGroup;
	Ogre::Viewport *mVp;
	
	Ogre::RenderTexture *mRenderTexture;

	Ogre::RaySceneQuery *mTerrainRayQuery;	

	Ogre::Camera *mCamera;
	Ogre::SceneNode *mTerrain, *mEntitiesNode;

	

	std::list<Ogre::Light*> mStaticLights;
	std::vector<Ogre::SceneNode *> mTerrainEntities;
	std::map<std::string, std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>> mInfoObjects;

	Ogre::TexturePtr mGameTexture;


	Serialize::ObservableList<Entity::Entity, Serialize::ContainerPolicy::masterOnly, Ogre::SceneNode *, std::string, Ogre::Entity*> mEntities;
    std::list<Entity::Entity *> mEntityRemoveQueue;

    std::list<SceneListener *> mSceneListeners;

    UniqueComponentCollector<BaseSceneComponent> mSceneComponents;
    Resources::Shading::ShaderCollector mShaderCollector;

    
    std::string mHeightmap;   

};

}
}


