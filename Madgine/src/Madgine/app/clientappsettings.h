#pragma once

#include "appsettings.h"

#include "Interfaces/window/windowapi.h"

namespace Engine
{
	namespace App
	{
		/**
		 * Contains the information needed to start the Application.
		 */
		struct ClientAppSettings : public AppSettings
		{

			Window::WindowSettings mWindowSettings;

			/**
			 * (optional) might be set to specify an InputHandler. Otherwise the default one will be created.
			 */
			Input::InputHandler* mInput = nullptr;
		};
	}
}
