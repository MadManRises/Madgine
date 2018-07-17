#pragma once

#include "../guisystem.h"

namespace MyGUI
{
	struct MouseButton;
	class Widget;
	class Gui;
	class OgrePlatform;
	class OgreRenderManager;
	class LayoutManager;
	class ResourceManager;
	class InputManager;
}

namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			class MADGINE_CLIENT_EXPORT MyGUILauncher : public GUISystem, public Ogre::FrameListener
			{
			public:
				MyGUILauncher(App::ClientApplication &app);
				~MyGUILauncher();

				bool preInit() override;
				void finalize() override;

				virtual int go() override;

				virtual bool singleFrame() override;

				// Inherited via GUISystem
				void injectKeyPress(const KeyEventArgs& arg) override;
				void injectKeyRelease(const KeyEventArgs& arg) override;
				void injectMousePress(const MouseEventArgs& arg) override;
				void injectMouseRelease(const MouseEventArgs& arg) override;
				void injectMouseMove(const MouseEventArgs& arg) override;


				bool isCursorVisible() override;
				void setCursorVisibility(bool v) override;
				void setCursorPosition(const Ogre::Vector2& pos) override;
				Ogre::Vector2 getCursorPosition() override;
				Ogre::Vector2 getScreenSize() override;


				

				std::array<float, 2> widgetRelative(MyGUI::Widget* w, int left = -1, int top = -1) const;
				std::array<float, 2> relativeMoveDelta(MyGUI::Widget* w) const;

				void destroy(MyGUI::Widget* w);

				virtual void setWindowProperties(bool fullscreen, unsigned int width, unsigned int height) override;
				virtual void resizeWindow() override;

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
				std::unique_ptr<Input::InputHandler> mInputHolder;
				Input::InputHandler *mInput;

				HWND mHwnd;

				MyGUI::Gui* mGUI;
				MyGUI::OgrePlatform* mPlatform;
				MyGUI::LayoutManager* mLayoutManager;
				MyGUI::ResourceManager* mResourceManager;
				MyGUI::InputManager* mInputManager;
				MyGUI::OgreRenderManager* mRenderManager;


				Ogre::Viewport* mViewport;
				Ogre::Camera* mCamera;
				Ogre::RenderWindow *mWindow;

				MyGUI::Widget* mInternRootWindow;

				float mScrollWheel;

				Ogre::Vector2 mMousePosition;
				Ogre::Vector2 mMoveDelta;
			};
		}
	}
}
