#pragma once

#include "application.h"


namespace Engine
{
	namespace App
	{


		class MADGINE_CLIENT_EXPORT ClientApplication : public Application
		{
		public:
			ClientApplication();
			~ClientApplication();

			void setup(const ClientAppSettings &settings);

			const ClientAppSettings &settings();

			virtual KeyValueMapList maps() override;

			GUI::GUISystem &gui(bool = true);

			ClientApplication &getSelf(bool = true);

			bool init() override;
			void finalize() override;

		private:
			std::unique_ptr<GUI::GUISystem> mGUI;


			const ClientAppSettings *mSettings;
		};

	}
}
