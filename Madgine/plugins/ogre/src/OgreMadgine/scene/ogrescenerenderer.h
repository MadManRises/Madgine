#pragma once

#include "Madgine/scripting/types/globalapicomponent.h"

#include "Madgine/scene/entity/masks.h"


//#include "Madgine/resources/ogretextureptr.h"

namespace Engine {


	class OGREMADGINE_EXPORT OgreSceneRenderer : public Scripting::GlobalAPIComponent<OgreSceneRenderer> {
	public:
		OgreSceneRenderer(App::Application & app);

		virtual bool init() override;
		virtual void finalize() override;

		Ogre::SceneManager *getSceneManager();

		Ogre::SceneNode *createEntityNode();


		//Ogre::Ray mousePointToRay(const Ogre::Vector2& mousePos) const;

		/*bool mousePointToTerrainPoint(const Ogre::Vector2& mousePos,
			Ogre::Vector3& result, Ogre::uint32 mask = Scene::Entity::Masks::TERRAIN_MASK);*/
		bool rayToTerrainPoint(const Ogre::Ray& ray, Ogre::Vector3& result, Ogre::uint32 mask = Scene::Entity::Masks::TERRAIN_MASK);

		bool RaycastFromPoint(const Ogre::Ray& ray, Ogre::Vector3& result, Ogre::uint32 mask);

		static void GetMeshInformation(const Ogre::MeshPtr mesh, size_t& vertex_count,
			Ogre::Vector3*& vertices, size_t& index_count, unsigned long*& indices,
			const Ogre::Vector3& position, const Ogre::Quaternion& orient,
			const Ogre::Vector3& scale);
		void getTerrainMeshInformation(size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count,
			unsigned int*& indices);


		void setGameTextureSize(const Ogre::Vector2& size);
		Ogre::TexturePtr& getGameTexture();
		Ogre::SceneNode* camera() const;
		Ogre::SceneManager* getSceneManager() const;
		Ogre::Image getScreenshot() const;
		Ogre::Viewport* getViewport() const;
		Ogre::RenderTarget* getRenderTarget() const;

		bool isUsingHeightmap() const;
		const std::vector<Ogre::SceneNode*>& terrainEntities() const;
		const Ogre::SceneNode* terrain() const;

	protected:
		void createCamera();


		/*Math::Bounds getSceneBounds() const;
		Math::Bounds getRasterizedSceneBounds();

		Ogre::Vector3 rasterizePoint(const Ogre::Vector3& v);
		Ogre::Vector2 rasterizePoint(const Ogre::Vector2& v);

		Ogre::Vector2 relToScenePos(const Ogre::Vector2& v);

		void writeStaticScene(Serialize::SerializeOutStream& out) const;
		void readStaticScene(Serialize::SerializeInStream& in);
		void readTerrain(Serialize::SerializeInStream& in);*/

	private:
		Ogre::Root *mRoot;
		Ogre::SceneManager *mSceneMgr;

		Ogre::SceneNode *mEntitiesNode;

		Ogre::SceneNode* mCameraNode;

		

		Ogre::RaySceneQuery* mTerrainRayQuery;


		Ogre::SceneNode *mTerrain;

		std::vector<Ogre::SceneNode *> mTerrainEntities;

		//    Resources::Shading::ShaderCollector mShaderCollector;

		std::string mHeightmap;

	};


}