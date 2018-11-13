#pragma once

#include "OIS.h"
#include <Madgine/input/inputhandler.h>

namespace Engine
{
	namespace Input
	{
		class OISInputHandler : public OIS::KeyListener, public OIS::MouseListener,
			public InputHandler
		{
		public:
			OISInputHandler(Window::Window *window);
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

			void onResize(size_t width, size_t height);

			float mMouseScale;



			//OIS Input devices
			OIS::InputManager* mInputManager;
			OIS::Mouse* mMouse;
			OIS::Keyboard* mKeyboard;

			Window::Window* mWindow;			
		};
	}
}
