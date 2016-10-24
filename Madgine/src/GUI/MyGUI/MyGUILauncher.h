#pragma once

#include "GUI\GUISystem.h"

#ifdef _MSC_VER
#pragma warning (push, 0)
#endif
#include <MYGUI\MyGUI.h>
#include <MYGUI\MyGUI_OgrePlatform.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif


namespace Engine {
	namespace GUI {
		namespace MyGui {
			class MyGUIWindow;

			class MyGUILauncher : public GUISystem {
			public:
				MyGUILauncher(Ogre::RenderWindow *window, Ogre::SceneManager *sceneMgr);
				~MyGUILauncher();

				virtual void init() override;

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

				Ogre::Vector2 widgetRelative(MyGUI::Widget *w, int left = -1, int top = -1);
				Ogre::Vector2 relativeMoveDelta(MyGUI::Widget *w);

				void destroy(MyGUI::Widget *w);


			private:
				
				Ogre::unique_ptr<MyGUI::Gui> mGUI;
				Ogre::unique_ptr<MyGUI::OgrePlatform> mPlatform;
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