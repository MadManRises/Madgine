

#include "libinclude.h"

#include "Application.h"
#include "appsettings.h"
#include "GUI\MyGUI\MyGUILauncher.h"
#include "Util\UtilMethods.h"

#include "Input\OISInputHandler.h"

namespace Engine {

	API_IMPL(App::Application, &shutdown);

namespace App {

Application::Application() :
	mSettings(0),
	mShutDown(true),
	mWindow(0)
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

	_setup(new Input::OISInputHandler(mGUI.get(), mWindow));

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

	//mStory->methodCall("openLoadingScreen");
	mUI->openLoadingScreen();

	mLoader->load();

	mUI->init(); // Initialise all Handler

	mSceneMgr->init(); // Initialise all Scene-Components

	mStory->init();

	mConfig->applyLanguage();  // Set the Language in the Config to all Windows

	mRoot->addFrameListener(this);
}

int Application::go()
{
	mShutDown = false;

	if (!mStory->callMethodCatch("init"))
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

	mRoot = OGRE_MAKE_UNIQUE(Ogre::Root)(pluginsFile); // Creating Root

	mConfig = OGRE_MAKE_UNIQUE(ConfigSet)(mRoot.get(), "config.vs"); // Loading Config and configuring Root

	Util::UtilMethods::setup();

}

void Application::_setup(Input::InputHandler *input)
{
	mLoader = OGRE_MAKE_UNIQUE(Resources::ResourceLoader)();

	// Instantiate the Story class
	mStory = OGRE_MAKE_UNIQUE(Scripting::Story)(mLoader->scriptParser());

	mStory->addAPI(this);

	// Create SceneManager
	mSceneMgr = OGRE_MAKE_UNIQUE(OGRE::SceneManager)(mRoot.get());

	// Initialise GUISystem 
	mGUI = OGRE_MAKE_UNIQUE_BASE(GUI::MyGui::MyGUILauncher, GUI::GUISystem)(mWindow, mSceneMgr->getSceneManager());
	//mGUI = OGRE_MAKE_UNIQUE_FUNC(GUI::Cegui::CEGUILauncher, GUI::GUISystem)(); 

	// Create UIManager
	mUI = OGRE_MAKE_UNIQUE(UI::UIManager)(mWindow, mSceneMgr.get(), mGUI.get());

	mInput = Ogre::unique_ptr<Input::InputHandler>(input);

	mProfiler = OGRE_MAKE_UNIQUE(Util::Profiler)();
}



void Application::_cleanup()
{
	mInput.reset();
	mProfiler.reset();
	mLoader.reset();
	mUI.reset();
	mGUI.reset();
	mSceneMgr->finalize();
	mSceneMgr.reset();
	mStory.reset();
    mConfig.reset();
    mRoot.reset();
}



}
}

