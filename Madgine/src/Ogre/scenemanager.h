#pragma once

//#include "Ogre/scenecomponent.h"
#include "Resources/Shading/shadercollector.h"
#include "Scripting/Datatypes/Serialize/serializable.h"
#include "TextureComponent.h"
#include "Entity\masks.h"

namespace Engine {
namespace OGRE {



class MADGINE_EXPORT SceneManager : public Ogre::Singleton<SceneManager>, public Ogre::GeneralAllocatedObject, public Scripting::Serialize::Serializable {
public:
    SceneManager(Ogre::Root *root);
    ~SceneManager();

    void init();
	void finalize();

	void update(float timeSinceLastFrame, App::ContextMask mask);


    void addSceneListener(SceneListener *l);
    void removeSceneListener(SceneListener *l);


	size_t getComponentCount();

	
    void onLoad();


	void setGameTextureSize(const Ogre::Vector2 &size);
	TextureComponent &getGameTexture();
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

protected:
    virtual void save(Scripting::Serialize::SerializeOutStream &out) const override;
    virtual void load(Scripting::Serialize::SerializeInStream &in) override;


	virtual void createCamera(void);

	void createScene(Scripting::Serialize::SerializeInStream &in);
	void createTerrain(Ogre::SceneNode *terrain, Scripting::Serialize::SerializeInStream &in);


	void removeQueuedEntities();


	void clear();
    
	
    bool RaycastFromPoint(const Ogre::Ray &ray, Ogre::Vector3 &result, Ogre::uint32 mask);



private:

    Ogre::Root *mRoot;
	Ogre::SceneManager *mSceneMgr;
	Ogre::Viewport *mVp;
	Ogre::Camera *mCamera;
    Ogre::SceneNode *mTerrain, *mEntitiesNode;
	Ogre::TerrainGlobalOptions *mTerrainGlobals;
	Ogre::TerrainGroup *mTerrainGroup;
	Ogre::RenderTexture *mRenderTexture;

	Ogre::RaySceneQuery *mTerrainRayQuery;

	std::list<Ogre::Light*> mStaticLights;
	std::vector<Ogre::SceneNode *> mTerrainEntities;
	std::map<std::string, std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>> mInfoObjects;

	TextureComponent mGameTexture;


    std::list<Ogre::unique_ptr<Entity::Entity>> mEntities;
    std::list<Entity::Entity *> mEntityRemoveQueue;

    std::list<SceneListener *> mSceneListeners;

    Ogre::unique_ptr<UniqueComponentCollector<BaseSceneComponent>> mSceneComponents;
    Ogre::unique_ptr<Resources::Shading::ShaderCollector> mShaderCollector;

    
    std::string mHeightmap;


    bool mIsSceneLoaded;

    static const float sceneRasterSize;

};

}
}


