#pragma once


#include "../serialize/container/list.h"

#include "scenemanager.h"

#include "entity/masks.h"

#include "../resources/ogretextureptr.h"

#include "ogrelight.h"

namespace Engine
{
	namespace Scene
	{
		class OGREMADGINE_EXPORT OgreSceneManager :
			public SceneManager<OgreSceneManager>
		{
		public:
			OgreSceneManager(Ogre::Root* root);
			virtual ~OgreSceneManager();

			using Singleton<OgreSceneManager>::getSingleton;
			using Singleton<OgreSceneManager>::getSingletonPtr;

			bool init() override;
			void finalize() override;

			void setGameTextureSize(const Ogre::Vector2& size);
			Resources::OgreTexturePtr& getGameTexture();
			Ogre::SceneNode* camera() const;
			Ogre::SceneManager* getSceneManager() const;
			Ogre::Image getScreenshot() const;
			Ogre::Viewport* getViewport() const;
			Ogre::RenderTarget* getRenderTarget() const;

			bool isUsingHeightmap() const;
			const std::vector<Ogre::SceneNode*>& terrainEntities() const;
			const Ogre::SceneNode* terrain() const;
			Ogre::TerrainGroup* terrainGroup() const;

			Ogre::SceneNode* createEntityNode();

			Light* createLight() override;
			std::list<Light*> lights() override;


			Math::Bounds getSceneBounds() const;
			Math::Bounds getRasterizedSceneBounds();

			Ogre::Vector3 rasterizePoint(const Ogre::Vector3& v);
			Ogre::Vector2 rasterizePoint(const Ogre::Vector2& v);

			Ogre::Vector2 relToScenePos(const Ogre::Vector2& v);


			const std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>& getInfoObject(const std::string& name) const;

			static void GetMeshInformation(const Ogre::MeshPtr mesh, size_t& vertex_count,
			                               Ogre::Vector3*& vertices, size_t& index_count, unsigned long*& indices,
			                               const Ogre::Vector3& position, const Ogre::Quaternion& orient,
			                               const Ogre::Vector3& scale);
			void getTerrainMeshInformation(size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count,
			                               unsigned int*& indices);


			Ogre::Ray mousePointToRay(const Ogre::Vector2& mousePos) const;
			bool mousePointToTerrainPoint(const Ogre::Vector2& mousePos,
			                              Ogre::Vector3& result, Ogre::uint32 mask = Entity::Masks::TERRAIN_MASK);
			bool rayToTerrainPoint(const Ogre::Ray& ray, Ogre::Vector3& result, Ogre::uint32 mask = Entity::Masks::TERRAIN_MASK);


			void clear() override;

			void writeState(Serialize::SerializeOutStream& out) const override;
			void readState(Serialize::SerializeInStream& in) override;

			void readScene(Serialize::SerializeInStream& in);
			void writeScene(Serialize::SerializeOutStream& out) const;

			static constexpr const float sSceneRasterSize = .2f;

		protected:

			void createCamera();

			void writeStaticScene(Serialize::SerializeOutStream& out) const;
			void readStaticScene(Serialize::SerializeInStream& in);
			void readTerrain(Serialize::SerializeInStream& in);


			bool RaycastFromPoint(const Ogre::Ray& ray, Ogre::Vector3& result, Ogre::uint32 mask);


			std::tuple<Ogre::Light *> createLightData();

		private:

			Ogre::Root* mRoot;

			Ogre::SceneManager* mSceneMgr;
			Ogre::TerrainGlobalOptions* mTerrainGlobals;
			Ogre::TerrainGroup* mTerrainGroup;
			Ogre::Viewport* mVp;

			Ogre::RenderTexture* mRenderTexture;

			Ogre::RaySceneQuery* mTerrainRayQuery;

			Ogre::SceneNode* mCameraNode;
			Ogre::Camera* mCamera;
			Ogre::SceneNode *mTerrain, *mEntitiesNode;


			std::list<Ogre::Light*> mStaticLights;
			std::vector<Ogre::SceneNode *> mTerrainEntities;
			std::map<std::string, std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>> mInfoObjects;

			Resources::OgreTexturePtr mGameTexture;

			Serialize::ObservableList<OgreLight, Serialize::ContainerPolicies::masterOnly, Serialize::ParentCreator<
				                          decltype(&OgreSceneManager::createLightData), &OgreSceneManager::createLightData>>
			mLights;

			//    Resources::Shading::ShaderCollector mShaderCollector;


			std::string mHeightmap;
		};
	}
}
