#pragma once

#include "ProcessTalker.h"

#include "InputWrapper.h"

#include "ModuleLoader.h"

#include "ApplicationWatcher.h"

#include "ApplicationInfo.h"

class LogWatcher;
struct ApplicationInfo;

class ApplicationLauncher : public ProcessTalker<ApplicationMsg>, public Ogre::FrameListener {
public:

	ApplicationLauncher();
	~ApplicationLauncher();

	int exec();

	void load();
	void start();
	void stop();

			
	//Watcher::InputWrapper *input();



	// Geerbt über ProcessTalker
	virtual void receiveMessage(const ApplicationMsg & msg) override;

	void resizeWindow();


	virtual bool frameRenderingQueued(const Ogre::FrameEvent &fe) override;

	bool sendMsg(ApplicationCmd cmd);

private:

	ModuleLoader mLoader;

	Engine::App::AppSettings mSettings;
	Engine::App::Application *mApplication;

	LogWatcher *mOgreLog;

	InputWrapper mInput;

	ApplicationInfo &mAppInfo;

	ApplicationWatcher mWatcher;

	bool mRunning;

};
