#include "serverlib.h"

#include "serverapplication.h"

namespace Engine {
	namespace App {

		ServerApplication::ServerApplication() :
			mSceneManager(0)
		{
		}

		ServerApplication::~ServerApplication()
		{
			if (mSceneManager) {
				mSceneManager->finalize();
				delete mSceneManager;
			}
		}

		int ServerApplication::go()
		{
			bool run = true;
			while (run) {
				run = update(0);
			}
			return 0;
		}

		bool ServerApplication::init()
		{
			if (!Application::init())
				return false;

			mSceneManager->init();

			return true;
		}


		void ServerApplication::_setup()
		{
			Application::_setup();

			mSceneManager = new Scene::ServerSceneManager;
		}

		bool ServerApplication::fixedUpdate(float timeStep)
		{
			mSceneManager->update(timeStep, Engine::App::ContextMask::SceneContext);

			return true;
		}

	}
}