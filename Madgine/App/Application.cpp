

#include "libinclude.h"

#include "Application.h"
#include "appsettings.h"
#include "GUI\MyGUI\MyGUILauncher.h"
#include "Util\UtilMethods.h"

#include "OGRE\scenemanager.h"

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
	_cleanup();
}

void Application::setup(const std::string &pluginsFile, const std::string &windowName)
{
	_setupOgre(pluginsFile);

	mWindow = mRoot->initialise(true, windowName); // Create Application-Window

	_setup(new Input::OISInputHandler(mGUI, mWindow));

}

void Application::setupExternal(const std::string &pluginsFile, const std::string &windowName, int width, int height, const Ogre::NameValuePairList &parameters, Input::InputHandler *input)
{
	_setupOgre(pluginsFile);

	mRoot->initialise(false);

	mWindow = mRoot->createRenderWindow(windowName, width, height, false, &parameters);

	_setup(input);

}

void Application::init(const AppSettings & settings)
{
	mSettings = &settings;

	mLoader->setup(settings.mRootDir);

	mGUI->init();

	//mGlobalScope->methodCall("openLoadingScreen");
	mUI->openLoadingScreen();

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

	if (!mGlobalScope->callMethodCatch("init"))
		return -1;

	mRoot->startRendering();

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
	app.setup("plugins.cfg", settings.mWindowName);
	app.init(settings);
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

	{
		PROFILE("Input", "Rendering");
		mInput->update();
	}
	
	{
		PROFILE("RenderQueued", "Rendering");
		mUI->update(fe.timeSinceLastFrame);
	}

	{
		PROFILE("SafeCall", "Rendering");
		while (!mSafeCallQueue.empty()) {
			mSafeCallQueue.front()();
			mSafeCallQueue.pop();
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

void Application::_setupOgre(const std::string &pluginsFile)
{

	mRoot = OGRE_NEW Ogre::Root(pluginsFile); // Creating Root

	mConfig = OGRE_NEW ConfigSet(mRoot, "config.vs"); // Loading Config and configuring Root

	Util::UtilMethods::setup();

}

void Application::_setup(Input::InputHandler *input)
{
	mLoader = OGRE_NEW Resources::ResourceLoader();

	// Instantiate the GlobalScope class
	mGlobalScope = OGRE_NEW Scripting::GlobalScope(mLoader->scriptParser());

	mGlobalScope->addAPI(this);

	// Create SceneManager
	mSceneMgr = OGRE_NEW OGRE::SceneManager(mRoot);

	// Initialise GUISystem 
	mGUI = OGRE_NEW GUI::MyGui::MyGUILauncher(mWindow, mSceneMgr->getSceneManager());
	//mGUI = OGRE_MAKE_UNIQUE_FUNC(GUI::Cegui::CEGUILauncher, GUI::GUISystem)(); 

	// Create UIManager
	mUI = OGRE_NEW UI::UIManager(mWindow, mSceneMgr, mGUI);

	mInput = input;

	mProfiler = OGRE_NEW Util::Profiler();
}



void Application::_cleanup()
{
	if (mInput)
		delete mInput;
	if (mProfiler)
		delete mProfiler;
	if (mLoader)
		delete mLoader;
	if (mUI)
		delete mUI;
	if (mGUI)
		delete mGUI;
	if (mSceneMgr) {
		mSceneMgr->finalize();
		delete mSceneMgr;
	}
	if (mGlobalScope)
		delete mGlobalScope;
	if (mConfig)
		delete mConfig;
    if (mRoot)
		delete mRoot;
}



}
}

