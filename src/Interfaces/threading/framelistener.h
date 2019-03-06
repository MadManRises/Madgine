#pragma once

#include "../../Madgine/scene/contextmasks.h"


namespace Engine
{
	namespace Threading
	{
		class INTERFACES_EXPORT FrameListener
		{
		public:
			virtual ~FrameListener() = default;

			virtual bool frameStarted(std::chrono::microseconds timeSinceLastFrame) { return true; }
			virtual bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) { return true; }
			virtual bool frameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) { return true; }
			virtual bool frameEnded(std::chrono::microseconds timeSinceLastFrame) { return true; }
		};
	}
}
