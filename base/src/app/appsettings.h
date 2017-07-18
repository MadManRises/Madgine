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
	AppSettings(const Scripting::LuaTable &table) :
		mTable(table)
	{

	}

	AppSettings()		
	{

	}

	std::string mAppName;

	Scripting::LuaTable mTable;

};


}
}
