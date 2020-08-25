#pragma once

#include "contextmasks.h"
#include "Modules/threading/taskqueue.h"

namespace Engine
{
	namespace Threading
	{
		struct MADGINE_CLIENT_EXPORT FrameLoop : Threading::TaskQueue
		{
		public:
			
			FrameLoop();
			FrameLoop(const FrameLoop &) = delete;
			virtual ~FrameLoop();

			FrameLoop &operator=(const FrameLoop&) = delete;

			virtual bool singleFrame(std::chrono::microseconds timeSinceLastFrame = 0us);

			std::chrono::nanoseconds fixedRemainder() const;

			void addFrameListener(FrameListener* listener);
			void removeFrameListener(FrameListener* listener);

			void shutdown();
			bool isShutdown() const;

		protected:

			bool sendFrameStarted(std::chrono::microseconds timeSinceLastFrame);
			bool sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, ContextMask context = ContextMask::SceneContext);
			bool sendFrameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, ContextMask context = ContextMask::SceneContext);
			bool sendFrameEnded(std::chrono::microseconds timeSinceLastFrame);

			virtual std::optional<Threading::TaskTracker> fetch_on_idle() override;

		private:			

			
			std::vector<FrameListener*> mListeners;

			std::chrono::high_resolution_clock::time_point mLastFrame;

			std::chrono::microseconds mTimeBank{ 0 };

			static constexpr std::chrono::microseconds FIXED_TIMESTEP{ 15000 };
		};

		
	}
}