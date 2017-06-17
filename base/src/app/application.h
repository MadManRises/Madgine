#pragma once


#include "SignalSlot\ConnectionStore.h"

#include "Scripting\Types\globalapicomponentbase.h"
#include "Scripting\Types\scope.h"

namespace Engine {
	namespace App {

		class MADGINE_BASE_EXPORT Application : public Singleton<Application>, public Scripting::Scope<Application, Scripting::GlobalAPIComponentBase> {
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
			void setup(const AppSettings &settings);

			/**
			* May only be called after a call to setup().
			* Initializes all Madgine-Components.
			*/
			virtual bool init() override;

			virtual void finalize() override;

			/**
			* Tries to call the script-method "init", which must be implemented in a script-file or in a Scripting::GlobalAPI, and to start the Ogre-Renderloop.
			* If "init" is not found, <code>-1</code> is returned.
			* Otherwise the Renderloop will be started.
			* After the Renderloop finished, all game components will be cleared.
			* Note: this method will <b>not</b> return, until the Application is shutdown.
			*
			* @return <code>0</code>, if the Application is started and shutdown properly; <code>-1</code> otherwise
			*/
			virtual int go() = 0;

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
			template <class App, class Settings>
			static int run(const Settings &settings)
			{
				App app;
				app.setup(settings);
				if (!app.init()) return -1;
				int result = app.go();
				app.finalize();
				return result;
			}


			/**
			* This will be called by Ogre whenever a new frame was sent to and gets rendered by the GPU. It returns <code>false</code>, if the Application was shutdown().
			* Otherwise it will update all Profilers for frame-profiling, capture input from the Input::InputHandler, update the UI::UIManager and perform all tasks given by callSafe().
			* This is the toplevel method of the Madgine, that should recursively update all elements that need update per frame.
			*
			* @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
			* @param fe holds the time since the last frame
			*/
			virtual bool update(float timeSinceLastFrame);

			float fixedRemainder();

			bool isShutdown();

			float getFPS();

			void addFrameListener(FrameListener *listener);
			void removeFrameListener(FrameListener *listener);

		protected:
			virtual void _clear();
			
			virtual bool fixedUpdate(float timeStep);			

			bool sendFrameStarted(float timeSinceLastFrame);
			bool sendFrameRenderingQueued(float timeSinceLastFrame);
			bool sendFrameEnded(float timeSinceLastFrame);

			Scripting::GlobalScope *globalScope();

			virtual lua_State *lua_state() = 0;
		private:

			bool mShutDown;

			Scripting::GlobalScope*      mGlobalScope;

			SignalSlot::ConnectionManager mConnectionManager;	

			std::unique_ptr<Util::StandardLog> mLog;

			float mTimeBank;

			std::list<FrameListener*> mListeners;

			static constexpr float FIXED_TIMESTEP = 0.015f;

		};


	}
}

