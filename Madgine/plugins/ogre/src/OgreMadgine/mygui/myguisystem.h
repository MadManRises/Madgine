#pragma once

#include "Madgine/gui/guisystem.h"

namespace Engine
{
	namespace GUI
	{
		
		class MyGUISystem : public GUISystem, public Ogre::WindowEventListener, public Ogre::FrameListener
		{
		public:
			MyGUISystem(App::ClientApplication &app);
			~MyGUISystem();

			void injectKeyPress(const KeyEventArgs& arg) override;
			void injectKeyRelease(const KeyEventArgs& arg) override;
			void injectMousePress(const MouseEventArgs& arg) override;
			void injectMouseRelease(const MouseEventArgs& arg) override;
			void injectMouseMove(const MouseEventArgs& arg) override;

			static MyGUI::MouseButton convertButton(MouseButton::MouseButton buttonID);
			static MouseButton::MouseButton convertButton(MyGUI::MouseButton buttonID);

			bool isCursorVisible() override;
			void setCursorVisibility(bool v) override;
			void setCursorPosition(const Vector2& pos) override;
			Vector2 getCursorPosition() override;
			Vector2 getScreenSize() override;

			bool preInit() override;
			int go() override;
			bool singleFrame() override;

			std::unique_ptr<Window> createWindow(const std::string &name) override;
			std::unique_ptr<Bar> createBar(const std::string &name) override;
			std::unique_ptr<Button> createButton(const std::string &name) override;
			std::unique_ptr<Checkbox> createCheckbox(const std::string &name) override;
			std::unique_ptr<Combobox> createCombobox(const std::string &name) override;
			std::unique_ptr<Label> createLabel(const std::string &name) override;
			std::unique_ptr<SceneWindow> createSceneWindow(const std::string &name) override;
			std::unique_ptr<TabWindow> createTabWindow(const std::string &name) override;
			std::unique_ptr<Textbox> createTextbox(const std::string &name) override;
			std::unique_ptr<Image> createImage(const std::string &name) override;

			MyGUI::Widget *rootWidget();

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


			
		private:
			std::unique_ptr<Ogre::Root> mRoot;
			Ogre::Camera *mCamera;
			Ogre::Viewport *mViewport;
			Ogre::RenderWindow *mWindow;			
			void *mHwnd;

			std::unique_ptr<Input::InputHandler> mInputHolder;
			Input::InputHandler *mInput;

			MyGUI::OgrePlatform *mPlatform;
			MyGUI::Gui *mGUI;
			MyGUI::InputManager* mInputManager;
			MyGUI::PointerManager *mPointerManager;
			MyGUI::OgreRenderManager *mRenderManager;
			MyGUI::LayoutManager* mLayoutManager;
			MyGUI::ResourceManager* mResourceManager;
			MyGUI::Widget *mRootWindow;

			float mScrollWheel;

			Vector2 mMousePosition;
			Vector2 mMoveDelta;

		};

	}
}
