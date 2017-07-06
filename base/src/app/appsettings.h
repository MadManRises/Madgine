#pragma once


namespace Engine{
namespace App{

/**
 * Contains the information needed to start the Application.
 */
struct AppSettings {

	/**
	 * Sets up default values for the settings.
	 */
	AppSettings(const std::pair<lua_State *, int> &state) :
		mState(state)
	{

	}

	AppSettings() :
		mState(std::make_pair(nullptr, 0))
	{

	}

	std::string mAppName;

	std::pair<lua_State *, int> mState;

};


}
}
