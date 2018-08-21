#include "../ogrelib.h"
#include "oisinputhandler.h"

#include "Madgine/input/inputevents.h"

namespace Engine
{
	namespace Input
	{
		OISInputHandler::OISInputHandler(Ogre::RenderWindow* window) :
			mWindow(window)
		{
			LOG("*** Initializing OIS ***");
			OIS::ParamList pl;
			size_t windowHnd = 0;
			std::ostringstream windowHndStr;

			window->getCustomAttribute("WINDOW", &windowHnd);
			windowHndStr << windowHnd;
			pl.insert(make_pair(std::string("WINDOW"), windowHndStr.str()));

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
			OISInputHandler::windowResized(window);
		}

		OISInputHandler::~OISInputHandler()
		{
			Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
		}

		MouseButton::MouseButton OISInputHandler::convertMouseButton(OIS::MouseButtonID id)
		{
			switch (id)
			{
			case OIS::MB_Left:
				return MouseButton::LEFT_BUTTON;
			case OIS::MB_Right:
				return MouseButton::RIGHT_BUTTON;
			case OIS::MB_Middle:
				return MouseButton::MIDDLE_BUTTON;
			default:
				throw 0;
			}
		}

		bool OISInputHandler::keyPressed(const OIS::KeyEvent& arg)
		{
			injectKeyPress({static_cast<Key>(arg.key), static_cast<char>(arg.text)});
			return true;
		}

		bool OISInputHandler::keyReleased(const OIS::KeyEvent& arg)
		{
			injectKeyRelease({static_cast<Key>(arg.key)});
			return true;
		}

		bool OISInputHandler::mousePressed(const OIS::MouseEvent& arg,
		                                   OIS::MouseButtonID id)
		{
			injectMousePress({
				{static_cast<float>(arg.state.X.abs), static_cast<float>(arg.state.Y.abs)},
				convertMouseButton(id)
			});
			return true;
		}

		bool OISInputHandler::mouseMoved(const OIS::MouseEvent& arg)
		{
			injectMouseMove({
				{static_cast<float>(arg.state.X.abs), static_cast<float>(arg.state.Y.abs)},
				{static_cast<float>(arg.state.X.rel * mMouseScale), static_cast<float>(arg.state.Y.rel * mMouseScale)},
				arg.state.Z.rel / 120.0f
			});
			return true;
		}

		bool OISInputHandler::mouseReleased(const OIS::MouseEvent& arg,
		                                    OIS::MouseButtonID id)
		{
			injectMouseRelease({
				{static_cast<float>(arg.state.X.abs), static_cast<float>(arg.state.Y.abs)},
				convertMouseButton(id)
			});
			return true;
		}

		void OISInputHandler::update()
		{
			//Need to capture/update each device
			mKeyboard->capture();
			mMouse->capture();
		}

		void OISInputHandler::windowResized(Ogre::RenderWindow* rw)
		{
			unsigned int width, height, depth;
			int left, top;
			rw->getMetrics(width, height, depth, left, top);

			const OIS::MouseState& ms = mMouse->getMouseState();
			ms.width = width;
			ms.height = height;

			mMouseScale = (width / 640.0f + height / 480.0f) / 2;
		}

		void OISInputHandler::windowClosed(Ogre::RenderWindow* rw)
		{
			if (mInputManager)
			{
				mInputManager->destroyInputObject(mMouse);
				mInputManager->destroyInputObject(mKeyboard);

				OIS::InputManager::destroyInputSystem(mInputManager);
				mInputManager = nullptr;
			}
		}
	}
}
