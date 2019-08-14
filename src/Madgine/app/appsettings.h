#pragma once

#include "Interfaces/window/windowapi.h"

namespace Engine
{
	namespace App
	{
		/**
		 * Contains the information needed to start the Application.
		 */
		struct AppSettings
		{
			std::string mAppName;

			bool mRunMain = true;

			Window::WindowSettings mWindowSettings;

			/**
			 * (optional) might be set to specify an InputHandler. Otherwise the default one will be created.
			 */
			void* mInput = nullptr;
		};
	}
}
