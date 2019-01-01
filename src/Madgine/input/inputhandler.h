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

			virtual void onResize(size_t width, size_t height);

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

RegisterType(Engine::Input::InputHandler);