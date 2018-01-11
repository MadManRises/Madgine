#pragma once

#include "app/application.h"
#include "servertimer.h"

#include "serialize/container/noparent.h"

namespace Engine
{
	namespace App
	{
		class MADGINE_SERVER_EXPORT ServerApplication : public Application
		{
		public:
			ServerApplication(const Scripting::LuaTable& table = {});
			virtual ~ServerApplication();

			void setup(const ServerAppSettings& settings);

			// Inherited via Application
			int go() override;

			bool init() override;

			void finalize() override;

			KeyValueMapList maps() override;

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
