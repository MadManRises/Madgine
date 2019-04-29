#pragma once


#include "input/inputcollector.h"

namespace Engine
{
	namespace Input
	{

		struct MADGINE_SDLINPUT_EXPORT SDLInputHandler :
			public InputHandlerComponent<SDLInputHandler>
		{
			SDLInputHandler(Window::Window *window);
			~SDLInputHandler();

		private:
			bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

			
		private:
		};

	}
}


RegisterType(Engine::Input::SDLInputHandler);