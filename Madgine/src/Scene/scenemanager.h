#pragma once

#include "Scene\scenecomponent.h"
#include "Resources/Shading/shadercollector.h"
#include "Scripting/Datatypes/Serialize/serializable.h"
#include "Entity\masks.h"
#include "Util\MadgineObject.h"

namespace Engine {
namespace Scene {

class MADGINE_EXPORT SceneManager : public Ogre::Singleton<SceneManager>, public Ogre::GeneralAllocatedObject, public Scripting::Serialize::Serializable, public Util::MadgineObject<SceneManager> {
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
	bool isSceneLoaded() const;
	const std::vector<Ogre::SceneNode*> &terrainEntities();
	const Ogre::SceneNode *terrain();
	Ogre::TerrainGroup *terrainGroup() const;

	Entity::Entity *createEntity(const std::string &name, const std::string &meshName, const std::string &behaviour);
	std::list<Entity::Entity *> entities() const;
	Entity::Entity *findEntity(const std::string &name, bool throwIfNotFound = false) const;
	void removeLater(Entity::Entity *e);



	Math::Bounds getSceneBounds();
	Math::Bounds getRasterizedSceneBounds();

	Ogre::Vector3 rasterizePoint(const Ogre::Vector3 &v);
	Ogre::Vector2 rasterizePoint(const Ogre::Vector2 &v);
	static float rasterSize();

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


	void saveComponentData(Scripting::Serialize::SerializeOutStream &out) const;
	void loadComponentData(Scripting::Serialize::SerializeInStream &in);

	std::set<BaseSceneComponent*> getComponents();
	std::set<SceneListener*> getListeners();

	void clear();

protected:
    virtual void save(Scripting::Serialize::SerializeOutStream &out) const override;
    virtual void load(Scripting::Serialize::SerializeInStream &in) override;


	virtual void createCamera(void);

	void createScene(Scripting::Serialize::SerializeInStream &in);
	void createTerrain(Ogre::SceneNode *terrain, Scripting::Serialize::SerializeInStream &in);


	void removeQueuedEntities();
		
    bool RaycastFromPoint(const Ogre::Ray &ray, Ogre::Vector3 &result, Ogre::uint32 mask);



private:

    Ogre::Root *mRoot;

	Ogre::SceneManager *mSceneMgr;
	Ogre::TerrainGlobalOptions *mTerrainGlobals;
	Ogre::TerrainGroup *mTerrainGroup;
	Ogre::Viewport *mVp;
	
	Ogre::RenderTexture *mRenderTexture;

	bool mIsSceneLoaded;

	Ogre::RaySceneQuery *mTerrainRayQuery;	

	Ogre::Camera *mCamera;
	Ogre::SceneNode *mTerrain, *mEntitiesNode;

	

	std::list<Ogre::Light*> mStaticLights;
	std::vector<Ogre::SceneNode *> mTerrainEntities;
	std::map<std::string, std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>> mInfoObjects;

	Ogre::TexturePtr mGameTexture;


    std::list<Ogre::unique_ptr<Entity::Entity>> mEntities;
    std::list<Entity::Entity *> mEntityRemoveQueue;

    std::list<SceneListener *> mSceneListeners;

    UniqueComponentCollector<BaseSceneComponent> mSceneComponents;
    Resources::Shading::ShaderCollector mShaderCollector;

    
    std::string mHeightmap;


    

    static const float sceneRasterSize;

};

}
}


