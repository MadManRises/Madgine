#pragma once

#include "application.h"
#include "servertimer.h"

#include "../serialize/container/noparent.h"

namespace Engine
{
	namespace App
	{
		class MADGINE_SERVER_EXPORT ServerApplication : public Application
		{
		public:
			ServerApplication(Root &root);
			virtual ~ServerApplication();

			void setup(const ServerAppSettings& settings);

			// Inherited via Application
			int go() override;

			bool singleFrame(float timeSinceLastFrame) override;

			bool init() override;

			void finalize() override;

			KeyValueMapList maps() override;

			virtual Scene::SceneManagerBase& sceneMgr() override;
			virtual Scene::SceneComponentBase& getSceneComponent(size_t) override;

		protected:

			bool fixedUpdate(float timeStep) override;
			bool update(float timeSinceLastFrame) override;


		private:
			Serialize::noparent_unique_ptr<Scene::ServerSceneManager> mSceneManager;
			ServerTimer mTimer;
			const ServerAppSettings* mSettings;
		};
	}
}
