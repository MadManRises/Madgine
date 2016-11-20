#pragma once

#include "Watcher\ObjectsWatcher.h"
#include "Watcher\PerformanceWatcher.h"

class ModuleLoader;

class ApplicationWatcher{

public:
	ApplicationWatcher();



	void clear();


				

	void update();

private:				
		
	PerformanceWatcher mPerformanceWatcher;
	ObjectsWatcher mObjectsWatcher;
	
	Ogre::RenderTarget *mSceneRenderWindow;
	Ogre::RenderWindow *mGuiRenderWindow;

};
