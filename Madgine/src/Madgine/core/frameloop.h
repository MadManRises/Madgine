#pragma once

#include "../scene/contextmasks.h"

namespace Engine
{
	namespace Core
	{
		class MADGINE_BASE_EXPORT FrameLoop
		{
		public:
			
			FrameLoop();
			virtual ~FrameLoop() = default;

			virtual int go();

			virtual bool init();
			virtual void finalize();

			virtual bool singleFrame();

			float fixedRemainder() const;

			void addFrameListener(FrameListener* listener);
			void removeFrameListener(FrameListener* listener);

		protected:
			bool sendFrameStarted(float timeSinceLastFrame);
			bool sendFrameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameFixedUpdate(float timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameEnded(float timeSinceLastFrame);
			
		private:
			std::vector<FrameListener*> mListeners;

			float mTimeBank;

			static constexpr float FIXED_TIMESTEP = 0.015f;
		};

		
	}
}