#pragma once

#include "Watcher\ObjectsWatcher.h"
#include "Watcher\PerformanceWatcher.h"

class ModuleLoader;

class ApplicationWatcher{

public:
	ApplicationWatcher();



private:				
		
	PerformanceWatcher mPerformanceWatcher;
	ObjectsWatcher mObjectsWatcher;
	
	Ogre::RenderTarget *mSceneRenderWindow;
	Ogre::RenderWindow *mGuiRenderWindow;

};
