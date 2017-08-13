

#include "madginelib.h"

#include "ogreapplication.h"
#include "ogreappsettings.h"
#include "gui/mygui/myguilauncher.h"

#include "scene/ogrescenemanager.h"

#include "ui/UIManager.h"
#include "configset.h"
#include "resources/resourceloader.h"
#include "util/profiler.h"

#include "input/oisinputhandler.h"

#include "OgreWindowEventUtilities.h"

#include "scripting/types/globalscope.h"


namespace Engine {

namespace App {

	OgreApplication::OgreApplication() :
	mSettings(nullptr),
		mPaused(false),
	mWindow(nullptr),
	mRoot(nullptr),
	mSceneMgr(nullptr),
	mGUI(nullptr),
	mUI(nullptr),
	mLoader(nullptr),
	mConfig(nullptr),
	mInput(nullptr)
{

}

	OgreApplication::~OgreApplication()
{
	if (mInput && !mSettings->mInput)
		delete mInput;
	if (mUI)
		delete mUI;
	if (mGUI)
		delete mGUI;
	if (mSceneMgr)
		delete mSceneMgr;
	if (mLoader)
		delete mLoader;
	if (mConfig)
		delete mConfig;
	if (mRoot)
		delete mRoot;
}

void OgreApplication::setup(const OgreAppSettings &settings)
{
	mSettings = &settings;

	_setupOgre();

	if (mSettings->mUseExternalSettings) {
		mRoot->initialise(false);

		mWindow = mRoot->createRenderWindow(mSettings->mWindowName, mSettings->mWindowWidth, mSettings->mWindowHeight, false, &mSettings->mWindowParameters);
	}
	else {
		mWindow = mRoot->initialise(true, mSettings->mWindowName); // Create Application-Window
	}

	mWindow->getCustomAttribute("WINDOW", &mHwnd);	

	mLoader = OGRE_NEW Resources::ResourceLoader(mSettings->mRootDir);

	Application::setup(settings);	

	// Create SceneManagerBase
	mSceneMgr = new Scene::OgreSceneManager(mRoot);

	mWindow->update();

	// Initialise GUISystem 
	mGUI = OGRE_NEW GUI::MyGui::MyGUILauncher(mWindow, mSceneMgr->getSceneManager());
	//mGUI = OGRE_MAKE_UNIQUE_FUNC(GUI::Cegui::CEGUILauncher, GUI::GUISystem)();
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, mGUI);

	// Create UIManager
	mUI = new UI::UIManager(mGUI);

	if (mSettings->mInput)
		mInput = mSettings->mInput;
	else
		mInput = new Input::OISInputHandler(mGUI, mWindow);
}

bool OgreApplication::init()
{
	
	mLoader->loadScripts();

	if (!Application::init())
		return false;

	if (!mGUI->init())
		return false;

	mRoot->addFrameListener(this);
	
	if (!mUI->preInit())
		return false;

	std::pair<bool, Scripting::ArgumentList> res = globalScope()->callMethodIfAvailable("afterViewInit");
	if (res.first && !res.second.empty() && (!res.second.front().is<bool>() || !res.second.front().as<bool>()))
		return false;

	mLoader->load();

	
	if (!mSceneMgr->init())
		return false; // Initialise all Scene-Components
	

	if (!mUI->init())
		return false; // Initialise all Handler
	


	mConfig->applyLanguage();  // Set the Language in the Config to all Windows

	return true;
}

void OgreApplication::finalize() {
	
	mUI->finalize();
	
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, mGUI);
	mGUI->finalize();
	
	mSceneMgr->finalize();
	
	Application::finalize();
}

int OgreApplication::go()
{
	Application::go();
	mPaused = false;

	if (!globalScope()->callMethodCatch("main").first) {
		return -1;
	}

	mRoot->startRendering();

	_clear();

	return 0;
}

void OgreApplication::shutdown()
{
	mConfig->save();
	Application::shutdown();
}

bool OgreApplication::frameStarted(const Ogre::FrameEvent & fe)
{
	if (!sendFrameStarted(fe.timeSinceLastFrame))
		return false;

	if (mWindow->isClosed())
		return false;
	
	return true;
}


bool OgreApplication::update(float timeSinceLastFrame) {
	Ogre::WindowEventUtilities::messagePump();
	return mRoot->renderOneFrame();
}

bool OgreApplication::frameRenderingQueued(const Ogre::FrameEvent & fe)
{
	
	if (mWindow->isClosed() || !Application::update(fe.timeSinceLastFrame)) {		
		return false;
	}

	if (!mPaused){
		
		try {
			PROFILE("UIManager");
			mUI->update(fe.timeSinceLastFrame);
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during UI-Update!");
			LOG_EXCEPTION(e);
		}

		try {
			PROFILE("SceneManager");
			mSceneMgr->update(fe.timeSinceLastFrame, mUI->currentContext());
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during Scene-Update!");
			LOG_EXCEPTION(e);
		}

		try {
			PROFILE("GUI");
			mGUI->update(fe.timeSinceLastFrame);
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during GUI-Update!");
			LOG_EXCEPTION(e);
		}

		try{
			PROFILE("Input");
			mInput->update();
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during Input!" );
			LOG_EXCEPTION(e);
		}

	}

	return true;
}

bool OgreApplication::frameEnded(const Ogre::FrameEvent & fe)
{

	if (!sendFrameEnded(fe.timeSinceLastFrame))
		return false;

	if (mWindow->isClosed())
		return false;

	return true;
}

bool OgreApplication::fixedUpdate(float timeStep)
{
	if (!mPaused) {

		try {
			PROFILE("SceneManager");
			mSceneMgr->fixedUpdate(timeStep, mUI->currentContext());
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during Scene-Update!");
			LOG_EXCEPTION(e);
		}

		try {
			PROFILE("UIManager");
			mUI->fixedUpdate(timeStep);
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during UI-Update!");
			LOG_EXCEPTION(e);
		}

	}

	return true;
}


void OgreApplication::setWindowProperties(bool fullscreen, unsigned int width, unsigned int height)
{
	mWindow->setFullscreen(fullscreen, width, height);
	resizeWindow();
}

Ogre::RenderWindow * OgreApplication::renderWindow()
{
	return mWindow;
}


void OgreApplication::_clear()
{
	mSceneMgr->clear();
	Application::_clear();
	mUI->clear();
}

void OgreApplication::_setupOgre()
{

	mRoot = OGRE_NEW Ogre::Root(mSettings->mPluginsFile); // Creating Root

	mConfig = OGRE_NEW ConfigSet(mRoot, "config.vs"); // Loading Config and configuring Root

}

void OgreApplication::resizeWindow()
{
	if (mWindow) {
		mWindow->windowMovedOrResized();
		Ogre::WindowEventUtilities::WindowEventListeners::iterator index,
			start = Ogre::WindowEventUtilities::_msListeners.lower_bound(mWindow),
			end = Ogre::WindowEventUtilities::_msListeners.upper_bound(mWindow);
		for (index = start; index != end; ++index)
			(index->second)->windowResized(mWindow);
	}
}

void OgreApplication::renderFrame()
{
	mGUI->renderSingleFrame();
	mWindow->update();
}

Scripting::KeyValueMapList OgreApplication::maps()
{
	return Application::maps().merge(mSceneMgr, mUI);
}

}
}

