#pragma once



#include "Scripting\Types\api.h"

namespace Engine {
namespace App {

/**
 * The Root-Class for the Madgine. 
 * This class is responsible for setting up the global parameters of the application and creating and initializing all Components of the Madgine.
 * Thats the class, that needs to be created in the user <code>main</code> method.
 * The Application can also be paused (e.g. in a multithreaded environment).
 * It implements Singleton, so there can only be one Application at a time per process.
 *
 */
class OGREMADGINE_EXPORT Application : public Ogre::Singleton<Application>,
	public Ogre::FrameListener, public Scripting::API<Application> {
public:
	/**
	 * Creates the Application.
	 * 
	 */
    Application();
	/**
	 * Deletes all objects created by the Application.
	 *
	 */
	virtual ~Application();

	/**
	 * Loads all plugins listed in <code>settings.mPluginsFile</code>.
	 * If no name is given "plugins.cfg" is used by default.
	 * Creates a RenderWindow with the given name and all Madgine-Components.
	 * If an InputHandler is passed, it will be used instead of the default one.
	 * The root-directory for the Application-Resources will be set according to the <code>settings.mRootDir</code>
	 *
	 * @param settings all necessary information to setup the Application
	 */
	virtual void setup(const AppSettings &settings);

	/**
	 * May only be called after a call to setup().
	 * Initializes all Madgine-Components.
	 */
	virtual void init();
	
	/**
	 * Tries to call the script-method "init", which must be implemented in a script-file or in a Scripting::GlobalAPI, and to start the Ogre-Renderloop. 
	 * If "init" is not found, <code>-1</code> is returned.
	 * Otherwise the Renderloop will be started. 
	 * After the Renderloop finished, all game components will be cleared.
	 * Note: this method will <b>not</b> return, until the Application is shutdown.
	 *
	 * @return <code>0</code>, if the Application is started and shutdown properly; <code>-1</code> otherwise
	 */
	virtual int go();
	/**
	 * Marks the Application as shutdown. This causes the Renderloop to return within the next frame.
	 */
	virtual void shutdown();

	/**
	 * Convenience method, that creates the Application of type T (which defaults to Application), calls setup(), init() and go() with the given <code>settings</code> and returns the result of the call to go().
	 *
	 * @return result of the call to go()
	 * @param settings the settings for the Application
	 */
	template <class T = Application>
	static int run(const AppSettings &settings)
	{
		T app;
		app.setup(settings);
		app.init();
		return app.go();
	}

	/**
	 * Enqueues a task to be performed within the end of the current frame. This can be used for two purposes:
	 * - To modify a container while iterating over it (e.g. remove an entity inside of Entity::update(), which is called by iterating over all entities)
	 * - To insert calls, that are not thread-safe, from another thread
	 *
	 * @param f a functional with the task to be executed
	 */
	virtual void callSafe(std::function<void()> f);

	/**
	 * Sets the properties of the Renderwindow. Might have unexpected effects, when used as embedded window.
	 *
	 * @param fullscreen flag indicating, if the window should be shown in fullscreen
	 * @param width the preferred width for the Renderwindow
	 * @param height the preferred height of the Renderwindow
	 */
	virtual void setWindowProperties(bool fullscreen, unsigned int width, unsigned int height);

	/**
	 * Returns the Renderwindow of the Application.
	 *
	 * @return the Ogre::RenderWindow, in which the application is rendered.
	 */
	virtual Ogre::RenderWindow *renderWindow();

	/**
	 * For embedded Applications. Resizes the Game-Components to the current size of the Renderwindow. Will be called automatically in a non-embedded environment.
	 */
	virtual void resizeWindow();

	/**
	 * Renders a single frame of the Application. Useful for rolling custom renderloops or updating screen during long calculations.(e.g. Loading Screen)
	 */
	virtual void renderFrame();

protected:
	/**
	 * This will be called by Ogre whenever a new frame is started. It returns <code>false</code>, if the Application was shutdown().
	 * Otherwise it will start all Profilers for frame-profiling.
	 *
	 * @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
	 * @param fe holds the time since the last frame
	 */
	virtual bool frameStarted(const Ogre::FrameEvent & fe) override;
	/**
	* This will be called by Ogre whenever a new frame was sent to and gets rendered by the GPU. It returns <code>false</code>, if the Application was shutdown().
	* Otherwise it will update all Profilers for frame-profiling, capture input from the Input::InputHandler, update the UI::UIManager and perform all tasks given by callSafe().
	* This is the toplevel method of the Madgine, that should recursively update all elements that need update per frame.
	*
	* @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
	* @param fe holds the time since the last frame
	*/
	virtual bool frameRenderingQueued(const Ogre::FrameEvent & fe) override;	
	/**
	* This will be called by Ogre whenever a frame is ended. It returns <code>false</code>, if the Application was shutdown().
	* Otherwise it will start all Profilers for frame-profiling.
	*
	* @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
	* @param fe holds the time since the last frame
	*/
	virtual bool frameEnded(const Ogre::FrameEvent & fe) override;

private:
	virtual void _clear();
	virtual void _setupOgre();
	virtual void _setup();
	

private:

	const AppSettings *mSettings;
	
	bool mShutDown;
	bool mPaused;

	Ogre::RenderWindow								   *mWindow;

    Ogre::Root*                       mRoot;
    Scene::OgreSceneManager*              mSceneMgr;
    GUI::GUISystem*                   mGUI;
	UI::UIManager*                    mUI;
	Resources::ResourceLoader*        mLoader;
    Scripting::ScriptingManager*      mScriptingMgr;
    ConfigSet*                        mConfig;
	Util::Util*                       mUtil;
	Input::InputHandler*              mInput;

	HWND mHwnd;

	std::queue<std::function<void()>> mSafeCallQueue;

	
};


}
}

