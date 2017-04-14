#include "serverlib.h"

#include "serverapplication.h"

namespace Engine {
	namespace App {

		ServerApplication::ServerApplication() :
			mSceneManager(nullptr)
		{
		}

		ServerApplication::~ServerApplication()
		{
			if (mSceneManager) {
				mSceneManager->finalize();
				delete mSceneManager;
			}
		}

		void ServerApplication::setup(const ServerAppSettings & settings)
		{
			mSettings = &settings;

			Application::setup(settings);
		}

		int ServerApplication::go()
		{
			Application::go();

			bool run = true;
			mTimer.reset();
			while (run) {
				float timeSinceLastFrame = mTimer.elapsed_us() / 1000000.0f;
				mTimer.start();
				run = sendFrameStarted(timeSinceLastFrame) &&
					update(timeSinceLastFrame) &&
					sendFrameEnded(timeSinceLastFrame);
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
			mSceneManager->fixedUpdate(timeStep, Engine::App::ContextMask::SceneContext);

			return true;
		}

	}
}