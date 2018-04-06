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
			class MyGUILauncher : public GUISystem
			{
			public:
				MyGUILauncher(Ogre::RenderWindow* window, Ogre::SceneManager* sceneMgr);
				~MyGUILauncher();

				bool init() override;
				void finalize() override;

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


				static MyGUI::MouseButton convertButton(MouseButton::MouseButton buttonID);
				static MouseButton::MouseButton convertButton(MyGUI::MouseButton buttonID);

				std::array<float, 2> widgetRelative(MyGUI::Widget* w, int left = -1, int top = -1) const;
				std::array<float, 2> relativeMoveDelta(MyGUI::Widget* w) const;

				void destroy(MyGUI::Widget* w);


			private:

				MyGUI::Gui* mGUI;
				MyGUI::OgrePlatform* mPlatform;
				MyGUI::LayoutManager* mLayoutManager;
				MyGUI::ResourceManager* mResourceManager;
				MyGUI::InputManager* mInputManager;
				MyGUI::OgreRenderManager* mRenderManager;


				Ogre::Viewport* mViewport;
				Ogre::Camera* mCamera;

				MyGUI::Widget* mInternRootWindow;

				float mScrollWheel;

				Ogre::Vector2 mMousePosition;
				Ogre::Vector2 mMoveDelta;
			};
		}
	}
}
