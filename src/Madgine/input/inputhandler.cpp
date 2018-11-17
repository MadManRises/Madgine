#include "../clientlib.h"

#include "inputhandler.h"

#include "inputlistener.h"

namespace Engine
{
	namespace Input
	{
		
		InputHandler::InputHandler() :
		mListener(nullptr)
		{
			
		}

		void InputHandler::setListener(InputListener *listener)
		{
			mListener = listener;
		}

		InputListener* InputHandler::listener()
		{
			return mListener;
		}


		void InputHandler::injectKeyPress(const KeyEventArgs& arg)
			{
				if (mListener)
					mListener->injectKeyPress(arg);
			}

			void InputHandler::injectKeyRelease(const KeyEventArgs& arg)
			{
				if (mListener)
					mListener->injectKeyRelease(arg);
			}

			void InputHandler::injectMousePress(const MouseEventArgs& arg)
			{
				if (mListener)
					mListener->injectMousePress(arg);
			}

			void InputHandler::injectMouseRelease(const MouseEventArgs& arg)
			{
				if (mListener)
					mListener->injectMouseRelease(arg);
			}

			void InputHandler::injectMouseMove(const MouseEventArgs& arg)
			{
				if (mListener)
					mListener->injectMouseMove(arg);
			}

	}
}