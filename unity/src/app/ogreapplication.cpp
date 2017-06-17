

#include "madginelib.h"

#include "OgreApplication.h"
#include "ogreappsettings.h"
#include "GUI\MyGUI\MyGUILauncher.h"
#include "Util\Util.h"

#include "Scene\ogrescenemanager.h"

#include "UI\UIManager.h"
#include "configset.h"
#include "Scripting\Types\scriptingmanager.h"
#include "Resources\ResourceLoader.h"
#include "Util\Profiler.h"

#include "Input\OISInputHandler.h"

#include "Network\networkmanager.h"

#include "Serialize\serializemanager.h"

#include "OgreWindowEventUtilities.h"

namespace Engine {

namespace App {

	OgreApplication::OgreApplication() :
	mSettings(0),
	mWindow(0),
	mRoot(0),
	mSceneMgr(0),
	mGUI(0),
	mUI(0),
	mLoader(0),
	mConfig(0),
	mUtil(0),
	mInput(0),
		mPaused(false),
		mSceneMgrInitialized(false),
		mGUIInitialized(false),
		mUIInitialized(false)
{

}

	OgreApplication::~OgreApplication()
{
	if (mInput && !mSettings->mInput)
		delete mInput;
	if (mUI) {
		if (mUIInitialized)
			mUI->finalize();
		delete mUI;
	}
	if (mGUI) {
		Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, mGUI);
		if (mGUIInitialized)
			mGUI->finalize();
		delete mGUI;
	}
	if (mSceneMgr) {
		if (mSceneMgrInitialized)
			mSceneMgr->finalize();
		delete mSceneMgr;
	}
	if (mLoader)
		delete mLoader;
	if (mUtil)
		delete mUtil;
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

	mUtil = OGRE_NEW Util::Util(mWindow);

	Application::setup(settings);
}

bool OgreApplication::init()
{
	mGUIInitialized = mGUI->init();
	if (!mGUIInitialized)
		return false;

	mRoot->addFrameListener(this);

	std::pair<bool, ValueType> res = Scripting::GlobalScope::getSingleton().callMethodCatch("afterViewInit");
	if (res.first && !res.second.isNull() && (!res.second.isBool() || !res.second.asBool()))
		return false;

	mLoader->load();

	mSceneMgrInitialized = mSceneMgr->init();
	if (!mSceneMgrInitialized)
		return false; // Initialise all Scene-Components
	
	mUIInitialized = mUI->init();
	if (!mUIInitialized)
		return false; // Initialise all Handler
	
	if (!Application::init())
		return false;

	mConfig->applyLanguage();  // Set the Language in the Config to all Windows

	return true;
}

int OgreApplication::go()
{
	Application::go();
	mPaused = false;

	if (!Scripting::GlobalScope::getSingleton().callMethodCatch("main").first)
		return -1;

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
	if (mWindow->isClosed())
		return false;

	mUtil->update();
	mUtil->profiler()->startProfiling("Frame");
	mUtil->profiler()->startProfiling("PreRender");

	return true;
}


bool OgreApplication::update(float timeSinceLastFrame) {
	Ogre::WindowEventUtilities::messagePump();
	return mRoot->renderOneFrame();
}

bool OgreApplication::frameRenderingQueued(const Ogre::FrameEvent & fe)
{
	mUtil->profiler()->stopProfiling(); // PreRender

	mUtil->profiler()->startProfiling("Rendering");

	if (mWindow->isClosed() || !Application::update(fe.timeSinceLastFrame)) {		
		mUtil->profiler()->stopProfiling(); // Frame
		return false;
	}

	if (!mPaused){

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
	mUtil->profiler()->stopProfiling(); // Rendering
	mUtil->profiler()->stopProfiling(); // Frame

	if (mWindow->isClosed())
		return false;

	return true;
}

bool OgreApplication::fixedUpdate(float timeStep)
{
	if (!mPaused) {

		try {
			PROFILE("SceneManager");
			mSceneMgr->update(timeStep, mUI->currentContext());
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during Scene-Update!");
			LOG_EXCEPTION(e);
		}

		try {
			PROFILE("UIManager");
			mUI->update(timeStep);
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

void OgreApplication::_setup()
{
	mLoader = OGRE_NEW Resources::ResourceLoader(mSettings->mRootDir);

	Application::_setup();

	// Create SceneManager
	mSceneMgr = OGRE_NEW Scene::OgreSceneManager(mRoot);

	mWindow->update();

	// Initialise GUISystem 
	mGUI = OGRE_NEW GUI::MyGui::MyGUILauncher(mWindow, mSceneMgr->getSceneManager());
	//mGUI = OGRE_MAKE_UNIQUE_FUNC(GUI::Cegui::CEGUILauncher, GUI::GUISystem)();
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, mGUI);

	// Create UIManager
	mUI = OGRE_NEW UI::UIManager(mGUI);

	if (mSettings->mInput)
		mInput = mSettings->mInput;
	else
		mInput = new Input::OISInputHandler(mGUI, mWindow);

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


}
}

