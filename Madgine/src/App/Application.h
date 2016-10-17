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
class MADGINE_EXPORT Application : public Ogre::Singleton<Application>,
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
	~Application();

	/**
	 * Loads all plugins listed in <code>pluginsFile</code>, creates a RenderWindow with the given name and all Madgine-Components.
	 *
	 * @param pluginsFile path to the file listing the plugins to be loaded
	 * @param windowName title of the Window, that will be created
	 */
	void setup(const std::string &pluginsFile, const std::string &windowName);
	/**
	 * Loads all plugins listed in <code>pluginsFile</code>, creates a RenderWindow with the given Parameters and all Madgine-Components. 
	 * If an InputHandler is passed, it will be used instead of the default one.
	 * Use this method, if you want to embed the Window in an already existing GUI.
	 *
	 * @param pluginsFile path to the file listing the plugins to be loaded
	 * @param windowName title of the Window, that will be created
	 * @param width width of the Window, that will be created
	 * @param height height of the Window, that will be created
	 * @param parameters additional Paramters of the Window, that will be created (see Ogre::Root::createRenderWindow)
	 * @param input (optional) alternate InputHandler to use
	 */
	void setupExternal(const std::string &pluginsFile, const std::string &windowName, int width, int height, const Ogre::NameValuePairList &parameters, Input::InputHandler *input = nullptr);
	/**
	 * May only be called after a call to setup() or setupExternal().
	 * Initializes all Madgine-Components. The root-directory for the Application-Resources will be set according to the <code>settings</code>.
	 *
	 * @param settings the settings for the Application
	 */
	void init(const AppSettings &settings);
	
	/**
	 * Tries to call the script-method "init", which must be implemented in a script-file or in a Scripting::GlobalAPI, and to start the Ogre-Renderloop. 
	 * If "init" is not found, <code>-1</code> is returned.
	 * Otherwise the Renderloop will be started. 
	 * Note: this method will <b>not</b> return, until the Application is shutdown.
	 *
	 * @return <code>0</code>, if the Application is started and shutdown properly; <code>-1</code> otherwise
	 */
	int go();
	/**
	 * Marks the Application as shutdown. This causes the Renderloop to return within the next frame.
	 */
	void shutdown();

	/**
	 * Convenience method, that calls setup(), init() and go() with the given <code>settings</code> and returns the result of the call to go().
	 *
	 * @return result of the call to go()
	 * @param settings the settings for the Application
	 */
	static int run(const AppSettings &settings);

	/**
	 * Enqueues a task to be performed within the end of the current frame. This can be used for two purposes:
	 * - To modify a container while iterating over it (e.g. remove an entity inside of Entity::update(), which is called by iterating over all entities)
	 * - To insert calls from another thread, that are not thread-safe
	 *
	 * @param f a functional with the task to be executed
	 */
	void callSafe(std::function<void()> f);

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
	void _cleanup();
	void _setupOgre(const std::string &pluginsFile);
	void _setup(Input::InputHandler *input = nullptr);

private:

	Ogre::RenderWindow								   *mWindow;

    Ogre::Root*                       mRoot;
    OGRE::SceneManager*               mSceneMgr;
    GUI::GUISystem*                   mGUI;
	UI::UIManager*                    mUI;
	Resources::ResourceLoader*        mLoader;
    Scripting::GlobalScope*                 mGlobalScope;
    ConfigSet*                        mConfig;
	Util::Profiler*                   mProfiler;
	Input::InputHandler*              mInput;

    const AppSettings *mSettings;

	std::queue<std::function<void()>> mSafeCallQueue;

	bool mShutDown;
};


}
}

