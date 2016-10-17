#include "libinclude.h"
#include "OISInputHandler.h"
#include "GUI\GUISystem.h"
#include "GUI\GUIEvents.h"
#include "Util\Profiler.h"
#include "Util\UtilMethods.h"

namespace Engine {
	namespace Input {
		OISInputHandler::OISInputHandler(GUI::GUISystem * gui, Ogre::RenderWindow *window) :
			mGUI(gui)
		{
			LOG("*** Initializing OIS ***");
			OIS::ParamList pl;
			size_t windowHnd = 0;
			std::ostringstream windowHndStr;

			window->getCustomAttribute("WINDOW", &windowHnd);
			windowHndStr << windowHnd;
			pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

			/*    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));*/

			mInputManager = OIS::InputManager::createInputSystem(pl);

			mKeyboard = static_cast<OIS::Keyboard *>(mInputManager->createInputObject(
				OIS::OISKeyboard, true));
			mMouse = static_cast<OIS::Mouse *>(mInputManager->createInputObject(
				OIS::OISMouse, true));

			mMouse->setEventCallback(this);
			mKeyboard->setEventCallback(this);

			Ogre::WindowEventUtilities::addWindowEventListener(window, this);

			//Set initial mouse clipping size
			windowResized(window);

		}

		GUI::MouseButton::MouseButton OISInputHandler::convertMouseButton(OIS::MouseButtonID id)
		{
			switch (id) {
			case OIS::MB_Left:
				return GUI::MouseButton::LEFT_BUTTON;
				break;
			case OIS::MB_Right:
				return GUI::MouseButton::RIGHT_BUTTON;
				break;
			case OIS::MB_Middle:
				return GUI::MouseButton::MIDDLE_BUTTON;
				break;
			default:
				throw 0;
			}
		}

		bool OISInputHandler::keyPressed(const OIS::KeyEvent &arg)
		{
			mGUI->injectKeyPress({ (GUI::Key)arg.key, (char)arg.text });
			return true;
		}

		bool OISInputHandler::keyReleased(const OIS::KeyEvent &arg)
		{
			mGUI->injectKeyRelease({ (GUI::Key)arg.key });
			return true;
		}

		bool OISInputHandler::mousePressed(const OIS::MouseEvent &arg,
			OIS::MouseButtonID id)
		{
			mGUI->injectMousePress({ Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs), convertMouseButton(id) });
			return true;
		}

		bool OISInputHandler::mouseMoved(const OIS::MouseEvent &arg)
		{
			mGUI->injectMouseMove({ Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs), Ogre::Vector2(arg.state.X.rel * mMouseScale, arg.state.Y.rel * mMouseScale), arg.state.Z.rel / 120.0f });
			return true;
		}

		bool OISInputHandler::mouseReleased(const OIS::MouseEvent &arg,
			OIS::MouseButtonID id)
		{
			mGUI->injectMouseRelease({ Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs), convertMouseButton(id) });
			return true;
		}

		void OISInputHandler::update()
		{

			//Need to capture/update each device
			mKeyboard->capture();
			mMouse->capture();

		}

		void OISInputHandler::windowResized(Ogre::RenderWindow *rw)
		{
			unsigned int width, height, depth;
			int left, top;
			rw->getMetrics(width, height, depth, left, top);

			const OIS::MouseState &ms = mMouse->getMouseState();
			ms.width = width;
			ms.height = height;

			mMouseScale = (width / 640.0f + height / 480.0f) / 2;
		}

		void OISInputHandler::windowClosed(Ogre::RenderWindow *rw)
		{
			if (mInputManager) {
				mInputManager->destroyInputObject(mMouse);
				mInputManager->destroyInputObject(mKeyboard);

				OIS::InputManager::destroyInputSystem(mInputManager);
				mInputManager = 0;
			}
		}

	}
}