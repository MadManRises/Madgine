#pragma once

#include "Interfaces/reflection/classname.h"
#include "Interfaces/threading/framelistener.h"

namespace Engine
{
	namespace Input
	{
		class MADGINE_CLIENT_EXPORT InputHandler : public Threading::FrameListener
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
			void injectPointerPress(const PointerEventArgs& arg);
			void injectPointerRelease(const PointerEventArgs& arg);
			void injectPointerMove(const PointerEventArgs& arg);


		private:
			InputListener* mListener;
		};
	}
}

RegisterType(Engine::Input::InputHandler);