#pragma once

#include "appsettings.h"

namespace Engine
{
	namespace App
	{
		/**
		 * Contains the information needed to start the Application.
		 */
		struct ClientAppSettings : public AppSettings
		{
			/**
			 * Sets up default values for the settings.
			 */
			ClientAppSettings() :
				mUseExternalSettings(false), mWindowWidth(0), mWindowHeight(0),
				mInput(nullptr)
			{
			}

			/**
			 * The name of the Application-window.
			 */
			std::string mWindowName;

			/**
			 * Needs to be set to specify additional setup-info for the Application-window.
			 */
			bool mUseExternalSettings;

			//Used For External
			/**
			 * The width of the Application-window.
			 */
			int mWindowWidth;

			/**
			* The height of the Application-window.
			*/
			int mWindowHeight;

			/**
			 * Special Properties of the Application-window.
			 */
			std::map<std::string, std::string> mWindowParameters;

			/**
			 * (optional) might be set to specify an InputHandler. Otherwise the default one will be created.
			 */
			Input::InputHandler* mInput;
		};
	}
}
