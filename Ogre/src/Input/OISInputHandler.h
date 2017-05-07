#pragma once

#include <OIS\OIS.h>
#include "InputHandler.h"

namespace Engine {
	namespace Input {
		class OISInputHandler : public Ogre::GeneralAllocatedObject, public OIS::KeyListener, public OIS::MouseListener, public Ogre::WindowEventListener, public InputHandler {
		public:
			OISInputHandler(GUI::GUISystem *gui, Ogre::RenderWindow *window);
			virtual ~OISInputHandler();

		private:
			GUI::MouseButton::MouseButton convertMouseButton(OIS::MouseButtonID id);

			// Inherited via KeyListener
			virtual bool keyPressed(const OIS::KeyEvent & arg) override;
			virtual bool keyReleased(const OIS::KeyEvent & arg) override;

			// Inherited via MouseListener
			virtual bool mouseMoved(const OIS::MouseEvent & arg) override;
			virtual bool mousePressed(const OIS::MouseEvent & arg, OIS::MouseButtonID id) override;
			virtual bool mouseReleased(const OIS::MouseEvent & arg, OIS::MouseButtonID id) override;

			virtual void update() override;

			virtual void windowResized(Ogre::RenderWindow * rw) override;

			virtual void windowClosed(Ogre::RenderWindow * rw) override;

			float mMouseScale;

			GUI::GUISystem *mGUI;

			//OIS Input devices
			OIS::InputManager          *mInputManager;
			OIS::Mouse                 *mMouse;
			OIS::Keyboard              *mKeyboard;

			Ogre::RenderWindow *mWindow;
		};
	}
}