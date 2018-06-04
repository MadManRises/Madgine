#pragma once

#include "Madgine/serialize/serializableunit.h"
#include "Madgine/serialize/container/observed.h"
#include "Madgine/serialize/container/action.h"



namespace Maditor {
	namespace Launcher {

		class AppStats : public Engine::Serialize::SerializableUnit<AppStats> {
		public:
			AppStats();

			void setApp(Engine::App::Application *app);
			
			void update();			

		private:
			Engine::Serialize::Observed<float> mAverageFPS;
			Engine::App::Application *mApp;

		};

	}
}
