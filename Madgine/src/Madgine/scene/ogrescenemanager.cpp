#include "../clientlib.h"

#include "ogrescenemanager.h"

#include "../math/bounds.h"

#include "../serialize/streams/serializestream.h"

#include "../util/process.h"

#include "../util/profiler.h"

#include "../serialize/serializemanager.h"

namespace Engine
{

	namespace Scene
	{
		OgreSceneManager::OgreSceneManager(App::Application &app, Ogre::Root* root) :
			SceneManager<Engine::Scene::OgreSceneManager>(app),
			mRoot(root),
			mSceneMgr(nullptr)
		{
			
		}

		OgreSceneManager::~OgreSceneManager()
		{
		}

		bool OgreSceneManager::init()
		{
			mSceneMgr = mRoot->getSceneManager("SceneTmp");

			
			return SceneManagerBase::init();
		}

		void OgreSceneManager::finalize()
		{
			SceneManagerBase::finalize();

			
		}

		

		



		void OgreSceneManager::writeState(Serialize::SerializeOutStream& out) const
		{
			//writeStaticScene(out);

			SceneManagerBase::writeState(out);
		}

		void OgreSceneManager::readState(Serialize::SerializeInStream& in)
		{
			//readStaticScene(in);

			SceneManagerBase::readState(in);
		}

		void OgreSceneManager::readScene(Serialize::SerializeInStream& in)
		{
			in.process().startSubProcess(1, "Loading Level...");

			readState(in);

			applySerializableMap(in.manager().slavesMap());

			in.process().endSubProcess();
		}

		void OgreSceneManager::writeScene(Serialize::SerializeOutStream& out) const
		{
			writeState(out);
		}

		






		std::tuple<Ogre::Light*> OgreSceneManager::createLightData()
		{
			return std::make_tuple(mSceneMgr->createLight());
		}

		Light* OgreSceneManager::createLight()
		{
			return &*mLights.emplace_back(mSceneMgr->createLight()).first;
		}

		std::list<Light*> OgreSceneManager::lights()
		{
			std::list<Light*> result;
			for (OgreLight& light : mLights)
			{
				result.push_back(&light);
			}
			return result;
		}


		
		

		void OgreSceneManager::clear()
		{
			mLights.clear();

			mStaticLights.clear();
			//mTerrainEntities.clear();
			mInfoObjects.clear();

			SceneManagerBase::clear();
		}


	}
}
