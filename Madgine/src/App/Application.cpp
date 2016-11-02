

#include "libinclude.h"

#include "Application.h"
#include "appsettings.h"
#include "GUI\MyGUI\MyGUILauncher.h"
#include "Util\UtilMethods.h"

#include "Scene\scenemanager.h"

#include "UI\UIManager.h"
#include "configset.h"
#include "Scripting\Types\globalscope.h"
#include "Resources\ResourceLoader.h"
#include "Util\Profiler.h"

#include "Input\OISInputHandler.h"

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
	mGlobalScope(0),
	mConfig(0),
	mProfiler(0),
	mInput(0)
{

}

Application::~Application()
{
	if (mInput && !mSettings->mInput)
		delete mInput;
	if (mProfiler)
		delete mProfiler;
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
	if (mGlobalScope) {
		mGlobalScope->finalize();
		delete mGlobalScope;
	}
	if (mLoader)
		delete mLoader;
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

	_setup();

}

void Application::init()
{
	mGUI->init();

	mGlobalScope->callMethodCatch("openMainLoadingScreen");

	mLoader->load();

	mUI->init(); // Initialise all Handler

	mSceneMgr->init(); // Initialise all Scene-Components

	mGlobalScope->init();

	mConfig->applyLanguage();  // Set the Language in the Config to all Windows

	mRoot->addFrameListener(this);
}

int Application::go()
{
	mShutDown = false;
	mPaused = false;

	if (!mGlobalScope->callMethodCatch("init"))
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

int Application::run(const AppSettings &settings)
{
	Application app;
	app.setup(settings);
	app.init();
	return app.go();
}

void Application::callSafe(std::function<void()> f)
{
	mSafeCallQueue.emplace(f);
}

bool Application::frameStarted(const Ogre::FrameEvent & fe)
{
	if (mWindow->isClosed() || mShutDown)
		return false;

	mProfiler->startProfiling("Frame");
	mProfiler->startProfiling("PreRender", "Frame");

	return true;
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent & fe)
{
	mProfiler->stopProfiling("PreRender");

	if (mWindow->isClosed() || mShutDown) {
		mProfiler->stopProfiling("Frame");
		return false;
	}

	mProfiler->startProfiling("Rendering", "Frame");

	if (!mPaused) {

		{
			PROFILE("Input", "Rendering");
			mInput->update();
		}

		{
			PROFILE("GUI", "Rendering");
			mGUI->update(fe.timeSinceLastFrame);
		}

		{
			PROFILE("UIManager", "Rendering");
			mUI->update(fe.timeSinceLastFrame);
		}

		{
			PROFILE("SceneManager", "Rendering");
			mSceneMgr->update(fe.timeSinceLastFrame, mUI->currentContext());
		}

		{
			PROFILE("SafeCall", "Rendering");
			while (!mSafeCallQueue.empty()) {
				mSafeCallQueue.front()();
				mSafeCallQueue.pop();
			}
		}

	}

	return true;
}

bool Application::frameEnded(const Ogre::FrameEvent & fe)
{
	mProfiler->stopProfiling("Rendering");
	mProfiler->stopProfiling("Frame");

	if (mWindow->isClosed() || mShutDown)
		return false;

	return true;
}


void Application::setWindowProperties(bool fullscreen, size_t width, size_t height)
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
	mGlobalScope->clear();
	mUI->clear();
}

void Application::_setupOgre()
{

	mRoot = OGRE_NEW Ogre::Root(mSettings->mPluginsFile); // Creating Root

	mConfig = OGRE_NEW ConfigSet(mRoot, "config.vs"); // Loading Config and configuring Root

	Util::UtilMethods::setup();

}

void Application::_setup()
{
	mLoader = OGRE_NEW Resources::ResourceLoader(mSettings->mRootDir);

	// Instantiate the GlobalScope class
	mGlobalScope = OGRE_NEW Scripting::GlobalScope(mLoader->scriptParser());

	mGlobalScope->addAPI(this);

	// Create SceneManager
	mSceneMgr = OGRE_NEW Scene::SceneManager(mRoot);

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

	mProfiler = OGRE_NEW Util::Profiler();
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

