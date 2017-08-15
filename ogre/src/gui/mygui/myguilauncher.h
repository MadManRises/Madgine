#pragma once

#include "gui/guisystem.h"

namespace MyGUI {
	struct MouseButton;
	class Widget;
	class Gui;
	class OgrePlatform;
	class OgreRenderManager;
	class LayoutManager;
	class ResourceManager;
	class InputManager;
}

namespace Engine {
	namespace GUI {
		namespace MyGui {

			class MyGUILauncher : public GUISystem {
			public:
				MyGUILauncher(Ogre::RenderWindow *window, Ogre::SceneManager *sceneMgr);
				~MyGUILauncher();

				virtual bool init() override;
				virtual void finalize() override;

				// Inherited via GUISystem
				virtual void injectKeyPress(const KeyEventArgs & arg) override;
				virtual void injectKeyRelease(const KeyEventArgs & arg) override;
				virtual void injectMousePress(const MouseEventArgs & arg) override;
				virtual void injectMouseRelease(const MouseEventArgs & arg) override;
				virtual void injectMouseMove(const MouseEventArgs & arg) override;

			

				virtual bool isCursorVisible() override;
				virtual void setCursorVisibility(bool v) override;
				virtual void setCursorPosition(const Ogre::Vector2 & pos) override;
				virtual Ogre::Vector2 getCursorPosition() override;
				virtual Ogre::Vector2 getScreenSize() override;


				static MyGUI::MouseButton convertButton(MouseButton::MouseButton buttonID);
				static MouseButton::MouseButton convertButton(MyGUI::MouseButton buttonID);

				std::array<float, 2> widgetRelative(MyGUI::Widget *w, int left = -1, int top = -1);
				std::array<float, 2> relativeMoveDelta(MyGUI::Widget *w);

				void destroy(MyGUI::Widget *w);


			private:
				
				MyGUI::Gui *mGUI;
				MyGUI::OgrePlatform *mPlatform;
				MyGUI::LayoutManager *mLayoutManager;
				MyGUI::ResourceManager *mResourceManager;
				MyGUI::InputManager *mInputManager;
				MyGUI::OgreRenderManager *mRenderManager;


				Ogre::Viewport *mViewport;
				Ogre::Camera *mCamera;

				MyGUI::Widget *mInternRootWindow;

				float mScrollWheel;

				Ogre::Vector2 mMousePosition;
				Ogre::Vector2 mMoveDelta;

			};
		}
	}
}