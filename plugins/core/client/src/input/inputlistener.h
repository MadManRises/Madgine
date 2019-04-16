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
			virtual bool injectPointerPress(const PointerEventArgs& arg) = 0;
			virtual bool injectPointerRelease(const PointerEventArgs& arg) = 0;
			virtual bool injectPointerMove(const PointerEventArgs& arg) = 0;

		};

	}
}