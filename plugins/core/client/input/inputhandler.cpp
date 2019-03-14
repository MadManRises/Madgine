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

		void InputHandler::onResize(size_t width, size_t height)
		{
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

			void InputHandler::injectPointerPress(const PointerEventArgs& arg)
			{
				if (mListener)
					mListener->injectPointerPress(arg);
			}

			void InputHandler::injectPointerRelease(const PointerEventArgs& arg)
			{
				if (mListener)
					mListener->injectPointerRelease(arg);
			}

			void InputHandler::injectPointerMove(const PointerEventArgs& arg)
			{
				if (mListener)
					mListener->injectPointerMove(arg);
			}

	}
}