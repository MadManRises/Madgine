#pragma once

#include "App\application.h"
#include "Scene\serverscenemanager.h"
#include "ServerTimer.h"
#include "serverappsettings.h"

namespace Engine {
	namespace App {

		class MADGINE_SERVER_EXPORT ServerApplication : public Application {
		public:
			ServerApplication();
			virtual ~ServerApplication();

			void setup(const ServerAppSettings &settings);

			// Inherited via Application
			virtual int go() override;

			virtual bool init() override;

		protected:
			virtual void _setup() override;

			virtual bool fixedUpdate(float timeStep) override;

		private:
			Scene::ServerSceneManager *mSceneManager;
			ServerTimer mTimer;
			const ServerAppSettings *mSettings;
		};

	}
}