#include "ogrelib.h"

#include "ogrescenerenderer.h"

#include "scene/entity/masks.h"



namespace Engine {

	API_IMPL(OgreSceneRenderer);

	OgreSceneRenderer::OgreSceneRenderer(App::Application & app) :
		Scripting::GlobalAPIComponent<OgreSceneRenderer>(app),
		mRoot(nullptr)
	{
		
	}

	bool OgreSceneRenderer::init() {
		mRoot = &Ogre::Root::getSingleton();

		mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "SceneTmp");


		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.8f, 0.8f, 0.8f));

		//mShaderCollector.init(mSceneMgr);

		Ogre::MovableObject::setDefaultQueryFlags(Scene::Entity::Masks::DEFAULT_MASK);

		return true;
	}

}