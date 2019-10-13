#pragma once

namespace Engine
{

	namespace Threading
	{
		struct MADGINE_CLIENT_EXPORT FrameListener
		{		
			virtual ~FrameListener() = default;

			virtual bool frameStarted(std::chrono::microseconds timeSinceLastFrame) { return true; }
			virtual bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) { return true; }
			virtual bool frameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) { return true; }
			virtual bool frameEnded(std::chrono::microseconds timeSinceLastFrame) { return true; }
		};
	}
}
