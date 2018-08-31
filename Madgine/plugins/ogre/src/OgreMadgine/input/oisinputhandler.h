#pragma once

#include "OIS.h"
#include <Madgine/input/inputhandler.h>

namespace Engine
{
	namespace Input
	{
		class OISInputHandler : public Ogre::GeneralAllocatedObject, public OIS::KeyListener, public OIS::MouseListener,
		                        public Ogre::WindowEventListener, public InputHandler
		{
		public:
			OISInputHandler(Ogre::RenderWindow* window);
			virtual ~OISInputHandler();

		private:
			static MouseButton::MouseButton convertMouseButton(OIS::MouseButtonID id);

			// Inherited via KeyListener
			bool keyPressed(const OIS::KeyEvent& arg) override;
			bool keyReleased(const OIS::KeyEvent& arg) override;

			// Inherited via MouseListener
			bool mouseMoved(const OIS::MouseEvent& arg) override;
			bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) override;
			bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id) override;

			void update() override;

			void windowResized(Ogre::RenderWindow* rw) override;

			void windowClosed(Ogre::RenderWindow* rw) override;

			float mMouseScale;



			//OIS Input devices
			OIS::InputManager* mInputManager;
			OIS::Mouse* mMouse;
			OIS::Keyboard* mKeyboard;

			Ogre::RenderWindow* mWindow;
		};
	}
}
