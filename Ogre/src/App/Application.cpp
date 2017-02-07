

#include "madginelib.h"

#include "Application.h"
#include "appsettings.h"
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

namespace Engine {

	API_IMPL(App::Application, &shutdown);

namespace App {

Application::Application() :
	mSettings(0),
	mShutDown(true),
	mPaused(false),
	mWindow(0),
	mRoot(0),
	mSceneMgr(0),
	mGUI(0),
	mUI(0),
	mLoader(0),
	mScriptingMgr(0),
	mConfig(0),
	mUtil(0),
	mInput(0)
{

}

Application::~Application()
{
	if (mInput && !mSettings->mInput)
		delete mInput;
	if (mUI) {
		mUI->finalize();
		delete mUI;
	}
	if (mGUI) {
		Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, mGUI);
		mGUI->finalize();
		delete mGUI;
	}
	if (mSceneMgr) {
		mSceneMgr->finalize();
		delete mSceneMgr;
	}
	if (mScriptingMgr) {
		mScriptingMgr->finalize();
		delete mScriptingMgr;
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

void Application::setup(const AppSettings &settings)
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

	_setup();

}

void Application::init()
{
	mGUI->init();

	mScriptingMgr->globalScope()->callMethodCatch("openMainLoadingScreen");

	mLoader->load();

	mSceneMgr->init(); // Initialise all Scene-Components
	
	mUI->init(); // Initialise all Handler
	
	mScriptingMgr->init();

	mConfig->applyLanguage();  // Set the Language in the Config to all Windows

	mRoot->addFrameListener(this);
}

int Application::go()
{
	mShutDown = false;
	mPaused = false;

	if (!mScriptingMgr->globalScope()->callMethodCatch("init"))
		return -1;

	mRoot->startRendering();

	_clear();

	return 0;
}

void Application::shutdown()
{
	mConfig->save();
	mShutDown = true;
}

void Application::callSafe(std::function<void()> f)
{
	mSafeCallQueue.emplace(f);
}

bool Application::frameStarted(const Ogre::FrameEvent & fe)
{
	if (mWindow->isClosed() || mShutDown)
		return false;

	mUtil->update();
	mUtil->profiler()->startProfiling("Frame");
	mUtil->profiler()->startProfiling("PreRender");

	return true;
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent & fe)
{
	mUtil->profiler()->stopProfiling(); // PreRender

	if (mWindow->isClosed() || mShutDown) {
		mUtil->profiler()->stopProfiling(); // Frame
		return false;
	}

	mUtil->profiler()->startProfiling("Rendering");

	if (!mPaused) {

		try{
			PROFILE("Input");
			mInput->update();
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during Input!" );
			LOG_EXCEPTION(e);
		}

		try{
			PROFILE("GUI");
			mGUI->update(fe.timeSinceLastFrame);
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during GUI-Update!");
			LOG_EXCEPTION(e);
		}

		try{
			PROFILE("UIManager");
			mUI->update(fe.timeSinceLastFrame);
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during UI-Update!");
			LOG_EXCEPTION(e);
		}

		try{
			PROFILE("SceneManager");
			mSceneMgr->update(fe.timeSinceLastFrame, mUI->currentContext());
		}
		catch (const std::exception &e) {
			LOG_ERROR("Unhandled Exception during Scene-Update!");
			LOG_EXCEPTION(e);
		}

		{
			PROFILE("SafeCall");
			while (!mSafeCallQueue.empty()) {
				try {
					mSafeCallQueue.front()();
				}
				catch (const std::exception &e) {
					LOG_ERROR("Unhandled Exception during SafeCall!");
					LOG_EXCEPTION(e);
				}
				mSafeCallQueue.pop();
			}
		}

	}

	return true;
}

bool Application::frameEnded(const Ogre::FrameEvent & fe)
{
	mUtil->profiler()->stopProfiling(); // Rendering
	mUtil->profiler()->stopProfiling(); // Frame

	if (mWindow->isClosed() || mShutDown)
		return false;

	return true;
}


void Application::setWindowProperties(bool fullscreen, unsigned int width, unsigned int height)
{
	mWindow->setFullscreen(fullscreen, width, height);
	resizeWindow();
}

Ogre::RenderWindow * Application::renderWindow()
{
	return mWindow;
}


void Application::_clear()
{
	mSceneMgr->clear();
	mScriptingMgr->clear();
	mUI->clear();
}

void Application::_setupOgre()
{

	mRoot = OGRE_NEW Ogre::Root(mSettings->mPluginsFile); // Creating Root

	mConfig = OGRE_NEW ConfigSet(mRoot, "config.vs"); // Loading Config and configuring Root

}

void Application::_setup()
{
	mLoader = OGRE_NEW Resources::ResourceLoader(mSettings->mRootDir);

	// Instantiate the GlobalScope class
	mScriptingMgr = new Scripting::ScriptingManager(mLoader->scriptParser());

	mScriptingMgr->globalScope()->addAPI(this);

	// Create SceneManager
	mSceneMgr = OGRE_NEW Scene::OgreSceneManager(mRoot);

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

void Application::resizeWindow()
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

void Application::renderFrame()
{
	mGUI->renderSingleFrame();
	mWindow->update();
}


}
}
