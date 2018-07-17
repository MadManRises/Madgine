#pragma once

#include "application.h"

namespace Engine
{
	namespace App
	{


		class MADGINE_CLIENT_EXPORT ClientApplication : public Application
		{
		public:
			ClientApplication(Core::Root &root);

			void setup(const ClientAppSettings &settings);

			const ClientAppSettings &settings();

			virtual KeyValueMapList maps() override;

			GUI::GUISystem &gui();

		private:
			GUI::GUISystem * mGUI;

			const ClientAppSettings *mSettings;
		};

	}
}
