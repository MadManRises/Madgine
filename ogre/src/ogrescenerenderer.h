#pragma once

#include "scripting/types/globalapicomponent.h"

namespace Engine {


	class OGREMADGINE_EXPORT OgreSceneRenderer : public Scripting::GlobalAPIComponent<OgreSceneRenderer> {
	public:
		OgreSceneRenderer(App::Application & app);

		virtual bool init() override;

	private:
		Ogre::Root *mRoot;
		Ogre::SceneManager *mSceneMgr;

		

	};


}