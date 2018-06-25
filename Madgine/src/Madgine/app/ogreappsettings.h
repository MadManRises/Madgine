#pragma once

#include "appsettings.h"

namespace Engine
{
	namespace App
	{
		/**
		 * Contains the information needed to start the Application.
		 */
		struct OgreAppSettings : public AppSettings
		{
			/**
			 * Sets up default values for the settings.
			 */
			OgreAppSettings() :
				mUseExternalSettings(false), mWindowWidth(0), mWindowHeight(0),
				mInput(nullptr)
			{
			}

			/**
			* The root-directory to look for resources.
			*/
			std::string mRootDir;

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
			Ogre::NameValuePairList mWindowParameters;

			/**
			 * (optional) might be set to specify an InputHandler. Otherwise the default one will be created.
			 */
			Input::InputHandler* mInput;
		};
	}
}
