#pragma once


#include "../scripting/types/scope.h"
#include "../madgineobject.h"

#include "../scripting/types/globalscopebase.h"

#include "../scripting/types/globalapicomponentcollector.h"

#include "../core/framelistener.h"

namespace Engine
{
	namespace App
	{
		/**
		 * \brief The Base-class for any Application that runs the Madgine.
		 */
		class MADGINE_BASE_EXPORT Application : public Scripting::Scope<Application, Scripting::GlobalScopeBase>,
			public MadgineObject, public Core::FrameListener
		{
		public:
			/**
			* Convenience method, that creates the Application of type T, calls setup(), init() and go() with the given <code>settings</code> and returns the result of the call to go().
			*
			* @return result of the call to go()
			* @param settings the settings for the Application
			*/
			template <class App, class Settings>
			static int run(const Settings& settings)
			{
				App app;
				app.setup(settings);
				if (!app.callInit()) return -1;
				int result = app.go();
				app.callFinalize();
				return result;
			}

			/**
			 * \brief Creates the Application
			 * \param state A pointer to the global LuaState to which this application will be registered.
			 */
			Application(Core::Root &root);
			
			/**
			 * \brief Deletes all objects created by the Application.
			 */
			virtual ~Application();

			/**
			* \brief Sets up a standard-log using the given App-name.
			*
			* @param settings all necessary information to setup the Application
			*/
			void setup(const AppSettings& settings, std::unique_ptr<Core::FrameLoop> &&loop = {});

			/**
			 * \brief starts the event loop of the madgine.
			 * This method will block until the event loop is stopped.
			 *
			* @return <code>0</code>, if the Application is started and shutdown properly; <code>-1</code> otherwise
			*/
			virtual int go();

			/**
			* Marks the Application as shutdown. This causes the event loop to return within the next frame.
			*/
			virtual void shutdown();

			/**
			* This will be called once every frame. It returns <code>false</code>, if the Application was shutdown().
			* Otherwise it will call frameRenderingQueued, perform a fixedUpdate if necessary and update all components that need a regular update:
			*  - The ConnectionManager
			*  - All GlobalApiComponents
			*  
			* This is the toplevel method of the Madgine, in which every derived class should recursively update all elements that need update per frame.
			*
			* @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
			* @param timeSinceLastFrame holds the time since the last frame
			*/
			virtual bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

			
			/**
			 * \brief Tells if the application is currently running.
			 * \return <code>true</code>, if the application is shutdown, so not running. <code>false</code>, otherwise.
			 */
			bool isShutdown() const;

			/**
			* \brief Retrieve the statistical frames-per-second value.
			* \return the current frames-per-second.
			*/
			float getFPS();


			KeyValueMapList maps() override;

			template <class T>
			T &getGlobalAPIComponent()
			{
				return static_cast<T&>(getGlobalAPIComponent(T::component_index()));
			}

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t, bool = true);

			template <class T>
			T &getSceneComponent()
			{
				return static_cast<T&>(getSceneComponent(T::component_index()));
			}

			Scene::SceneComponentBase &getSceneComponent(size_t, bool = true);
			Scene::SceneManager &sceneMgr(bool = true);
			Application &getSelf(bool = true);

			Util::Log &log();

			Core::Root &root();
			const Plugins::PluginManager &pluginMgr();
			Resources::ResourceManager &resources();


			/**
			* \brief Adds a FrameListener to the application.
			* \param listener the FrameListener to be added.
			*/
			void addFrameListener(Core::FrameListener* listener);
			/**
			* \brief Removes a FrameListener from the application.
			* \param listener the FrameListener to be removed.
			*/
			void removeFrameListener(Core::FrameListener* listener);

			void singleFrame();

		protected:
			virtual void clear();

			bool init() override;

			void finalize() override;


		private:

			Core::Root &mRoot;

			Scripting::GlobalAPICollector mGlobalAPIs;
			int mGlobalAPIInitCounter;

			std::unique_ptr<Util::StandardLog> mLog;

			std::unique_ptr<Core::FrameLoop> mLoop;			

			const AppSettings *mSettings;

			
		};
	}
}
