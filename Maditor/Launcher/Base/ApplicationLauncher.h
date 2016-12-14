#pragma once


#include "InputWrapper.h"

#include "ModuleLoader.h"

#include "ApplicationWatcher.h"

#include "ApplicationInfo.h"

#include "Network\networkmanager.h"

struct ApplicationInfo;

class ApplicationLauncher : public Ogre::FrameListener {
public:

	ApplicationLauncher();
	~ApplicationLauncher();

	int exec();

	void load();
	void start();
	void stop();	

	void resizeWindow();


	virtual bool frameRenderingQueued(const Ogre::FrameEvent &fe) override;

private:

	ModuleLoader mLoader;

	Engine::App::AppSettings mSettings;
	Engine::App::Application mApplication;
	Engine::Network::NetworkManager mNetwork;

	InputWrapper mInput;

	ApplicationInfo &mAppInfo;

	bool mRunning;

};
