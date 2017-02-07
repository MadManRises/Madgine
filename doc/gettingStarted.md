# Getting Started

This article describes all the neccessary steps to compile your first application with Madgine.

## Dependencies

The Madgine is built using CMake. It can be downloaded [here](https://cmake.org/download/). 

The following Libraries need to be available:

* [Ogre 1.8](http://www.ogre3d.org/download/sdk)
* Ogre Dependencies (e.g. [here](https://bitbucket.org/cabalistic/ogredeps), SDL2 not needed)

Also atleast one of the following Libraries needs to be available:

* [MyGUI](http://mygui.info/) Built with Ogre-support. Tested with version 3.2.2


*Recommended:*        
If you want to use the Maditor-IDE, the following libraries will also be neccessary:

* [Boost](.) vx.x or higher
* [Qt5](.)


## Building The Madgine

Download the source code from [GitHub](https://github.com/MadManRises/Madgine).

Set the CMake-Variables `OGRE_SDK` and `MYGUISDK` to the locations of your libraries.