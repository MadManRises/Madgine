#pragma once

namespace Engine
{
	namespace Input
	{
		
		class InputListener
		{
		public:
			virtual void injectKeyPress(const KeyEventArgs& arg) = 0;
			virtual void injectKeyRelease(const KeyEventArgs& arg) = 0;
			virtual void injectMousePress(const MouseEventArgs& arg) = 0;
			virtual void injectMouseRelease(const MouseEventArgs& arg) = 0;
			virtual void injectMouseMove(const MouseEventArgs& arg) = 0;

		};

	}
}