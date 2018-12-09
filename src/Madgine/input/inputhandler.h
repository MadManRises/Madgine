#pragma once

#include "Interfaces/reflection/classname.h"
#include "../core/framelistener.h"

namespace Engine
{
	namespace Input
	{
		class MADGINE_CLIENT_EXPORT InputHandler : public Core::FrameListener
		{
		public:
			InputHandler();
			virtual ~InputHandler() = default;
			
			void setListener(InputListener *listener);
			InputListener *listener();

		protected:
			void injectKeyPress(const KeyEventArgs& arg);
			void injectKeyRelease(const KeyEventArgs& arg);
			void injectMousePress(const MouseEventArgs& arg);
			void injectMouseRelease(const MouseEventArgs& arg);
			void injectMouseMove(const MouseEventArgs& arg);

		private:
			InputListener* mListener;
		};
	}
}

RegisterClass(Engine::Input::InputHandler);