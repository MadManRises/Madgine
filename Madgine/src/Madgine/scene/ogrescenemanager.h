#pragma once


#include "../serialize/container/list.h"

#include "scenemanager.h"

#include "entity/masks.h"


#include "ogrelight.h"

namespace Engine
{
	namespace Scene
	{
		class MADGINE_CLIENT_EXPORT OgreSceneManager :
			public SceneManager<OgreSceneManager>
		{
		public:
			OgreSceneManager(App::Application &app, Ogre::Root* root);
			virtual ~OgreSceneManager();

			bool init() override;
			void finalize() override;

			

			Ogre::SceneNode* createEntityNode();

			Light* createLight() override;
			std::list<Light*> lights() override;


			const std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>& getInfoObject(const std::string& name) const;

			

			
			void clear() override;

			void writeState(Serialize::SerializeOutStream& out) const override;
			void readState(Serialize::SerializeInStream& in) override;

			void readScene(Serialize::SerializeInStream& in);
			void writeScene(Serialize::SerializeOutStream& out) const;

			static constexpr const float sSceneRasterSize = .2f;

		protected:

			

			


			


			std::tuple<Ogre::Light *> createLightData();

		private:

			Ogre::Root* mRoot;

			Ogre::SceneManager* mSceneMgr;
			
			


			std::list<Ogre::Light*> mStaticLights;
			std::map<std::string, std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>> mInfoObjects;

			

			Serialize::ObservableList<OgreLight, Serialize::ContainerPolicies::masterOnly, Serialize::ParentCreator<
				                          decltype(&OgreSceneManager::createLightData), &OgreSceneManager::createLightData>>
			mLights;

		};
	}
}
