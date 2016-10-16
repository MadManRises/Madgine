#pragma once



#include "Scripting\Types\api.h"

namespace Engine {
namespace App {

/**
 * The Root-Class for the Engine. 
 * This class is responsible for setting up the global parameters of the application and creating and initializing all Components of the Engine.
 * Thats the class, that needs to be created in the user <code>main</code> method.
 *
 */
class MADGINE_EXPORT Application : public Ogre::Singleton<Application>,
	public Ogre::FrameListener, public Scripting::API<Application> {
public:
	/**
	 * Creates the Application with all the necessary information specified in <code>settings</code>. No Components are created.
	 * 
	 * \param settings contains information for the app (e.g. windowname, media-folder, see AppSettings). 
	 */
    Application();
	~Application();

	/**
	 * Creates and initializes all components, starts the ogre renderloop and thus hands over control to the UI::UIManager.
	 */
	void setup(const std::string &pluginsFile, const std::string &windowName);
	void setupExternal(const std::string &pluginsFile, const std::string &windowName, int width, int height, const Ogre::NameValuePairList &parameters, Input::InputHandler *input);
	void init(const AppSettings &settings);
	
	int go();
	void shutdown();

	static int run(const AppSettings &settings);

	void callSafe(std::function<void()> f);

protected:
	virtual bool frameStarted(const Ogre::FrameEvent & fe) override;
	virtual bool frameRenderingQueued(const Ogre::FrameEvent & fe) override;	
	virtual bool frameEnded(const Ogre::FrameEvent & fe) override;

private:
	void _cleanup();
	void _setupOgre(const std::string &pluginsFile);
	void _setup(Input::InputHandler *input);

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

