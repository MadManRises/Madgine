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

			void setup(const ClientAppSettings &settings);

			const ClientAppSettings &settings();

			virtual KeyValueMapList maps() override;

			GUI::GUISystem &gui(bool = true);

			ClientApplication &getSelf(bool = true);

		protected:
			virtual void loadFrameLoop(std::unique_ptr<Core::FrameLoop> &&loop) override;

		private:
			GUI::GUISystem * mGUI;

			const ClientAppSettings *mSettings;
		};

	}
}
