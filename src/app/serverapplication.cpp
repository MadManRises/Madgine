#include "../serverlib.h"

#include "serverapplication.h"
#include "serverappsettings.h"
#include "../scene/serverscenemanager.h"


namespace Engine
{
	namespace App
	{
		ServerApplication::ServerApplication(Scripting::LuaState *state) :
			Application(state),
			mSettings(nullptr)
		{
		}

		ServerApplication::~ServerApplication()
		{
		}

		void ServerApplication::setup(const ServerAppSettings& settings)
		{
			mSettings = &settings;

			Application::setup(settings);

			mSceneManager = Serialize::make_noparent_unique<Scene::ServerSceneManager>();
		}

		int ServerApplication::go()
		{
			Application::go();

			bool run = true;
			mTimer.reset();
			while (run)
			{
				float timeSinceLastFrame = mTimer.elapsed_us() / 1000000.0f;
				mTimer.start();
				run = singleFrame(timeSinceLastFrame);
				std::this_thread::yield();
			}
			return 0;
		}

		bool ServerApplication::init()
		{
			return Application::init() && mSceneManager->init();
		}

		void ServerApplication::finalize()
		{
			mSceneManager->finalize();
			Application::finalize();
		}

		bool ServerApplication::update(float timeSinceLastFrame)
		{
			mSceneManager->update(timeSinceLastFrame, Scene::ContextMask::SceneContext);
			return Application::update(timeSinceLastFrame);
		}

		bool ServerApplication::fixedUpdate(float timeStep)
		{
			mSceneManager->fixedUpdate(timeStep, Scene::ContextMask::SceneContext);
			return true;
		}

		KeyValueMapList ServerApplication::maps()
		{
			return Application::maps().merge(mSceneManager);
		}
	}
}
