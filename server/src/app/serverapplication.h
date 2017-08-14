#pragma once

#include "app/application.h"
#include "servertimer.h"

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

			virtual void finalize() override;

			virtual KeyValueMapList maps() override;

		protected:

			virtual bool fixedUpdate(float timeStep) override;


		private:
			Scene::ServerSceneManager *mSceneManager;
			ServerTimer mTimer;
			const ServerAppSettings *mSettings;
		};

	}
}