#pragma once


namespace Engine{
namespace App{

/**
 * Contains the information needed to start the Application.
 */
struct AppSettings {

    /**
     * The name of the Application-window.
     */
    std::string mWindowName;
    /**
     * The root-directory to look for resources
     */
    std::string mRootDir;

};


}
}
