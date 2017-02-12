#pragma once

#include "Resources/Shading/shadercollector.h"

#include "Entity\ogreentity.h"

#include "Serialize\Container\list.h"

#include "Scene\scenemanager.h"

#include "Scene\Entity\masks.h"

namespace Engine {
namespace Scene {

class OGREMADGINE_EXPORT OgreSceneManager : public Ogre::Singleton<OgreSceneManager>,
	public Ogre::GeneralAllocatedObject,
	public SceneManager
{
public:
    OgreSceneManager(Ogre::Root *root);
    virtual ~OgreSceneManager();

	using Ogre::Singleton<OgreSceneManager>::getSingleton;

    virtual void init() override;
	virtual void finalize() override;

	
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

	std::tuple<const Scripting::Parsing::EntityNode *, Ogre::SceneNode *, Ogre::Entity*> createEntityData(const std::string &behaviour, const std::string &name, const std::string &meshName);
	
	Entity::Entity *createEntity(const std::string &behaviour = "", const std::string &name = "", const std::string &meshName = "", const Scripting::ArgumentList &args = {});
	std::list<Entity::Entity *> entities();
	Entity::Entity *findEntity(const std::string &name);
	



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



	

	void clear();

	virtual void writeState(Serialize::SerializeOutStream &out) const override;
	virtual void readState(Serialize::SerializeInStream &in) override;

	void readScene(Serialize::SerializeInStream &in, bool callInit = false);
	void writeScene(Serialize::SerializeOutStream &out) const;

	static constexpr const float sSceneRasterSize = .2f;
	
	void makeLocalCopy(Entity::OgreEntity &e);
	void makeLocalCopy(Entity::OgreEntity &&e);

	void setEntitiesCallback(std::function<void(const Serialize::SerializableList<Entity::OgreEntity, const Scripting::Parsing::EntityNode *, Ogre::SceneNode *, Ogre::Entity*>::iterator &, int)> f);

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


	Serialize::ObservableList<Entity::OgreEntity, Serialize::ContainerPolicy::masterOnly, const Scripting::Parsing::EntityNode *, Ogre::SceneNode *, Ogre::Entity*> mEntities;
	std::list<Entity::OgreEntity> mLocalEntities;



    

    
    Resources::Shading::ShaderCollector mShaderCollector;

    
    std::string mHeightmap;   

};

}
}


