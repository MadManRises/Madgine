#pragma once

#include "../ois/includes/OIS.h"
#include <Madgine/input/inputcollector.h>
#include <Madgine/uniquecomponent/uniquecomponent.h>

namespace Engine
{
	namespace Input
	{
		class MADGINE_OIS_EXPORT OISInputHandler : public OIS::KeyListener, public OIS::MouseListener,
			public UniqueComponent<OISInputHandler, InputCollector>
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

RegisterClass(Engine::Input::OISInputHandler);
