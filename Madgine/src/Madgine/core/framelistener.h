#pragma once

#include "../scene/contextmasks.h"


namespace Engine
{
	namespace Core
	{
		class FrameListener
		{
		public:
			virtual ~FrameListener() = default;

			virtual bool frameStarted(float timeSinceLastFrame) { return true; }
			virtual bool frameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context) { return true; }
			virtual bool frameFixedUpdate(float timeSinceLastFrame, Scene::ContextMask context) { return true; }
			virtual bool frameEnded(float timeSinceLastFrame) { return true; }
		};
	}
}
