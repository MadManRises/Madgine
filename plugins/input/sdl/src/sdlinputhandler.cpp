#include "sdlinputlib.h"

#include "sdlinputhandler.h"

namespace Engine
{
	namespace Input
	{

		SDLInputHandler::SDLInputHandler(Window::Window * window)
		{			
		}

		SDLInputHandler::~SDLInputHandler()
		{			
		}

		bool SDLInputHandler::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{
			
			return true;
		}


	}
}