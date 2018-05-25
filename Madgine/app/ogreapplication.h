#pragma once

#include "application.h"

#include "../scripting/types/luastate.h"


#include "../serialize/container/noparent.h"

namespace Engine
{
	namespace App
	{
		/**
		 * The Root-Class for the Madgine. 
		 * This class is responsible for setting up the global parameters of the application and creating and initializing all Components of the Madgine.
		 * Thats the class, that needs to be created in the user <code>main</code> method.
		 * The Application can also be paused (e.g. in a multithreaded environment).
		 *
		 */
		class MADGINE_CLIENT_EXPORT OgreApplication : public Scripting::LuaState, public Application
		{
		public:
			/**
			 * Creates the Application.
			 * 
			 */
			OgreApplication(Plugins::PluginManager &pluginMgr);
			/**
			 * Deletes all objects created by the Application.
			 *
			 */
			virtual ~OgreApplication();

			/**
			 * Loads all plugins listed in <code>settings.mPluginsFile</code>.
			 * If no name is given "plugins.cfg" is used by default.
			 * Creates a RenderWindow with the given name and all Madgine-Components.
			 * If an InputHandler is passed, it will be used instead of the default one.
			 * The root-directory for the Application-Resources will be set according to the <code>settings.mRootDir</code>
			 *
			 * @param settings all necessary information to setup the Application
			 */
			void setup(const OgreAppSettings& settings);

			/**
			 * May only be called after a call to setup().
			 * Initializes all Madgine-Components.
			 */
			bool init() override;

			void finalize() override;

			/**
			 * Tries to call the script-method "init", which must be implemented in a script-file or in a Scripting::GlobalAPI, and to start the Ogre-Renderloop. 
			 * If "init" is not found, <code>-1</code> is returned.
			 * Otherwise the Renderloop will be started. 
			 * After the Renderloop finished, all game components will be cleared.
			 * Note: this method will <b>not</b> return, until the Application is shutdown.
			 *
			 * @return <code>0</code>, if the Application is started and shutdown properly; <code>-1</code> otherwise
			 */
			int go() override;


			/**
			 * Renders a single frame of the Application. Useful for rolling custom renderloops or updating screen during long calculations.(e.g. Loading Screen)
			 */
			virtual void renderFrame();

			void shutdown() override;


			bool update(float timeSinceLastFrame) override;

			virtual Scene::SceneManagerBase& sceneMgr() override;
			virtual Scene::SceneComponentBase& getSceneComponent(size_t) override;
			virtual UI::GuiHandlerBase& getGuiHandler(size_t) override;
			virtual UI::GameHandlerBase& getGameHandler(size_t) override;

			virtual bool singleFrame(float timeSinceLastFrame) override;

		protected:

			bool fixedUpdate(float timeStep) override;

			KeyValueMapList maps() override;

		private:
			void clear() override;

		private:

			const OgreAppSettings* mSettings;

			bool mPaused;

			
			std::unique_ptr<Resources::ResourceLoader> mLoader;
			Serialize::noparent_unique_ptr<Scene::OgreSceneManager> mSceneMgr;
			std::unique_ptr<GUI::GUISystem> mGUI;
			std::unique_ptr<UI::UIManager> mUI;
			
		};
	}
}
