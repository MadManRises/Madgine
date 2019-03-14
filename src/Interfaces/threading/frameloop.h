#pragma once

#include "../../Madgine/scene/contextmasks.h"
#include "../signalslot/taskqueue.h"

namespace Engine
{
	namespace Threading
	{
		struct INTERFACES_EXPORT FrameLoop : SignalSlot::TaskQueue
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
			bool isShutdown();

			virtual void queue(SignalSlot::TaskHandle &&task) override;
			virtual std::optional<SignalSlot::TaskHandle> fetch() override;
			virtual bool empty() override;

			void addSetupSteps(std::optional<SignalSlot::TaskHandle> &&init, std::optional<SignalSlot::TaskHandle> &&finalize = {});

		protected:

			bool sendFrameStarted(std::chrono::microseconds timeSinceLastFrame);
			bool sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameEnded(std::chrono::microseconds timeSinceLastFrame);

		private:
			bool mRunning = true;

			std::list<SignalSlot::TaskHandle> mTaskQueue;

			std::list<std::pair<std::optional<SignalSlot::TaskHandle>, std::optional<SignalSlot::TaskHandle>>> mSetupSteps;
			std::list<std::pair<std::optional<SignalSlot::TaskHandle>, std::optional<SignalSlot::TaskHandle>>>::iterator mSetupState;

			std::vector<FrameListener*> mListeners;

			std::chrono::high_resolution_clock::time_point mLastFrame;

			std::chrono::microseconds mTimeBank;

			static constexpr std::chrono::microseconds FIXED_TIMESTEP{ 15000 };
		};

		
	}
}