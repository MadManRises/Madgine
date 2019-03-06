#include "androidinputlib.h"

#include "androidinputhandler.h"
#include "eventbridge.h"

#include "Interfaces/debug/profiler/profiler.h"

#include <android/input.h>

namespace Engine
{
	namespace Input
	{

		extern AInputQueue *sQueue;

		AndroidInputHandler::AndroidInputHandler(Window::Window * window)
		{			
		}

		AndroidInputHandler::~AndroidInputHandler()
		{			
		}

		bool AndroidInputHandler::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{
			PROFILE();
			if (sQueue)
			{
				AInputEvent* event = NULL;
				while (AInputQueue_getEvent(sQueue, &event) >= 0) {
					if (AInputQueue_preDispatchEvent(sQueue, event)) {
						continue;
					}
					bool handled = false;
					switch (AInputEvent_getType(event))
					{

					}
					AInputQueue_finishEvent(sQueue, event, handled);
				}
			}
			
			return true;
		}


	}
}