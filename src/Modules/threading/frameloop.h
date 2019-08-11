#pragma once

#include "../../Madgine/scene/contextmasks.h"
#include "../signalslot/taskqueue.h"

namespace Engine
{
	namespace Threading
	{
		struct MODULES_EXPORT FrameLoop : SignalSlot::TaskQueue
		{
		public:
			
			FrameLoop();
			FrameLoop(const FrameLoop &) = delete;
			virtual ~FrameLoop() = default;

			FrameLoop &operator=(const FrameLoop&) = delete;

			virtual bool singleFrame(std::chrono::microseconds timeSinceLastFrame = 0us);

			std::chrono::nanoseconds fixedRemainder() const;

			void addFrameListener(FrameListener* listener);
			void removeFrameListener(FrameListener* listener);

			void shutdown();
			bool isShutdown() const;

			void addSetupSteps(SignalSlot::TaskHandle &&init, SignalSlot::TaskHandle &&finalize = {});

		protected:

			bool sendFrameStarted(std::chrono::microseconds timeSinceLastFrame);
			bool sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameEnded(std::chrono::microseconds timeSinceLastFrame);

			virtual std::optional<SignalSlot::TaskTracker> fetch(std::chrono::steady_clock::time_point &nextTask, int &idleCount) override;
			virtual std::optional<SignalSlot::TaskTracker> fetch_on_idle() override;
			virtual bool idle() const override;

		private:			

			std::list<std::pair<SignalSlot::TaskHandle, SignalSlot::TaskHandle>> mSetupSteps;
			std::list<std::pair<SignalSlot::TaskHandle, SignalSlot::TaskHandle>>::iterator mSetupState;

			std::vector<FrameListener*> mListeners;

			std::chrono::high_resolution_clock::time_point mLastFrame;

			std::chrono::microseconds mTimeBank{ 0 };

			static constexpr std::chrono::microseconds FIXED_TIMESTEP{ 15000 };
		};

		
	}
}