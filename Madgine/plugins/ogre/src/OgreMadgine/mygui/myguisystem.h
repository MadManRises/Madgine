#pragma once

#include "Madgine/gui/guisystem.h"

namespace Engine
{
	namespace GUI
	{
		
		class MyGUISystem : public GUISystem, public Ogre::FrameListener, public Ogre::LogListener
		{
		public:
			MyGUISystem(App::ClientApplication &app);
			~MyGUISystem();

			

			

			static MyGUI::MouseButton convertButton(Input::MouseButton::MouseButton buttonID);
			static Input::MouseButton::MouseButton convertButton(MyGUI::MouseButton buttonID);

			
			int go() override;			
			
		protected:
			/**
			* This will be called by Ogre whenever a new frame is started. It returns <code>false</code>, if the Application was shutdown().
			* Otherwise it will start all Profilers for frame-profiling.
			*
			* @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
			* @param fe holds the time since the last frame
			*/
			bool frameStarted(const Ogre::FrameEvent& fe) override;
			/**
			* This will be called by Ogre whenever a new frame was sent to and gets rendered by the GPU. It returns <code>false</code>, if the Application was shutdown().
			* Otherwise it will update all Profilers for frame-profiling, capture input from the Input::InputHandler, update the UI::UIManager and perform all tasks given by callSafe().
			* This is the toplevel method of the Madgine, that should recursively update all elements that need update per frame.
			*
			* @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
			* @param fe holds the time since the last frame
			*/
			bool frameRenderingQueued(const Ogre::FrameEvent& fe) override;
			/**
			* This will be called by Ogre whenever a frame is ended. It returns <code>false</code>, if the Application was shutdown().
			* Otherwise it will start all Profilers for frame-profiling.
			*
			* @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
			* @param fe holds the time since the last frame
			*/
			bool frameEnded(const Ogre::FrameEvent& fe) override;

			virtual void messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug,
				const Ogre::String& logName, bool& skipThisMessage) override;

			virtual bool init() override;
			
			
		private:
			std::unique_ptr<Ogre::Root> mRoot;


			

		};

	}
}
