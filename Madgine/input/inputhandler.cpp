#include "../clientlib.h"

#include "inputhandler.h"

#include "../gui/guisystem.h"

namespace Engine
{
	namespace Input
	{
		
		InputHandler::InputHandler() :
		mGUI(nullptr)
		{
			
		}

		void InputHandler::setSystem(GUI::GUISystem* gui)
		{
			mGUI = gui;
		}


			void InputHandler::injectKeyPress(const GUI::KeyEventArgs& arg)
			{
				if (mGUI)
					mGUI->injectKeyPress(arg);
			}

			void InputHandler::injectKeyRelease(const GUI::KeyEventArgs& arg)
			{
				if (mGUI)
					mGUI->injectKeyRelease(arg);
			}

			void InputHandler::injectMousePress(const GUI::MouseEventArgs& arg)
			{
				if (mGUI)
					mGUI->injectMousePress(arg);
			}

			void InputHandler::injectMouseRelease(const GUI::MouseEventArgs& arg)
			{
				if (mGUI)
					mGUI->injectMouseRelease(arg);
			}

			void InputHandler::injectMouseMove(const GUI::MouseEventArgs& arg)
			{
				if (mGUI)
					mGUI->injectMouseMove(arg);
			}

	}
}