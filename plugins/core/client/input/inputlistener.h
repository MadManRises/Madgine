#pragma once

namespace Engine
{
	namespace Input
	{
		
		class InputListener
		{
		public:
			virtual bool injectKeyPress(const KeyEventArgs& arg) = 0;
			virtual bool injectKeyRelease(const KeyEventArgs& arg) = 0;
			virtual bool injectMousePress(const MouseEventArgs& arg) = 0;
			virtual bool injectMouseRelease(const MouseEventArgs& arg) = 0;
			virtual bool injectMouseMove(const MouseEventArgs& arg) = 0;

		};

	}
}