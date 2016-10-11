#pragma once

#include "OGRE\scenemanager.h"

#include "GUI\GUISystem.h"
#include "UI\UIManager.h"
#include "configset.h"
#include "Scripting\Types\story.h"
#include "Resources\ResourceLoader.h"
#include "Util\Profiler.h"
#include "Input\InputHandler.h"

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

    Ogre::unique_ptr<Ogre::Root>                       mRoot;
    Ogre::unique_ptr<OGRE::SceneManager>               mSceneMgr;
    Ogre::unique_ptr<GUI::GUISystem>                   mGUI;
	Ogre::unique_ptr<UI::UIManager>		               mUI;
	Ogre::unique_ptr<Resources::ResourceLoader>		   mLoader;
    Ogre::unique_ptr<Scripting::Story>                 mStory;
    Ogre::unique_ptr<ConfigSet>                        mConfig;
	Ogre::unique_ptr<Util::Profiler>				   mProfiler;
	Ogre::unique_ptr<Input::InputHandler>           mInput;

    const AppSettings *mSettings;

	bool mShutDown;
};


}
}

