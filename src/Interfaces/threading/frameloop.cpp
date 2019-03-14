#include "../interfaceslib.h"

#include "frameloop.h"
#include "framelistener.h"

#include "../signalslot/taskqueue.h"

#include "../debug/profiler/profiler.h"

namespace Engine
{
	namespace Threading
	{
		FrameLoop::FrameLoop() :
			mTimeBank(0),			
			mLastFrame(std::chrono::high_resolution_clock::now()),
			mSetupState(mSetupSteps.begin())
		{
		}

		bool FrameLoop::singleFrame(std::chrono::microseconds timeSinceLastFrame)
		{
			return sendFrameStarted(timeSinceLastFrame) && sendFrameRenderingQueued(timeSinceLastFrame) && sendFrameEnded(timeSinceLastFrame);
		}

		void FrameLoop::addFrameListener(FrameListener* listener)
		{
			mListeners.push_back(listener);
		}

		void FrameLoop::removeFrameListener(FrameListener* listener)
		{
			mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
		}

		void FrameLoop::shutdown()
		{
			mRunning = false;
		}

		bool FrameLoop::isShutdown()
		{
			return !mRunning;
		}

		bool FrameLoop::sendFrameStarted(std::chrono::microseconds timeSinceLastFrame)
		{
			PROFILE();
			
			bool result = true;
			for (FrameListener* listener : mListeners)
				result &= listener->frameStarted(timeSinceLastFrame);
			return result;
		}

		bool FrameLoop::sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{
			PROFILE();
			bool result = true;
			for (FrameListener* listener : mListeners)
				result &= listener->frameRenderingQueued(timeSinceLastFrame, context);
			
			mTimeBank += timeSinceLastFrame;

			while (mTimeBank >= FIXED_TIMESTEP && result)
			{
				result &= sendFrameFixedUpdate(FIXED_TIMESTEP);
				mTimeBank -= FIXED_TIMESTEP;
			}

			return result;
		}

		bool FrameLoop::sendFrameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{
			PROFILE();
			bool result = true;
			for (FrameListener* listener : mListeners)
				result &= listener->frameFixedUpdate(timeSinceLastFrame, context);
			return result;
		}

		std::chrono::nanoseconds FrameLoop::fixedRemainder() const
		{
			return FIXED_TIMESTEP - mTimeBank;
		}

		bool FrameLoop::sendFrameEnded(std::chrono::microseconds timeSinceLastFrame)
		{
			PROFILE();
			bool result = true;
			for (FrameListener* listener : mListeners)
				result &= listener->frameEnded(timeSinceLastFrame);
			return result;
		}

		void FrameLoop::queue(SignalSlot::TaskHandle &&task)
		{
			mTaskQueue.emplace_back(std::move(task));
		}

		std::optional<SignalSlot::TaskHandle> FrameLoop::fetch()
		{
			if (mRunning)
			{
				while (mSetupState != mSetupSteps.end())
				{
					std::optional<SignalSlot::TaskHandle> init = std::move(mSetupState->first);
					++mSetupState;
					if (init)
						return init;
				}
			}
			if (!mTaskQueue.empty())
			{
				SignalSlot::TaskHandle task = std::move(mTaskQueue.front());
				mTaskQueue.pop_front();
				return task;
			}
			if (mRunning) 
			{
				return [this]() {
					auto now = std::chrono::high_resolution_clock::now();
					mRunning &= singleFrame(std::chrono::duration_cast<std::chrono::microseconds>(now - mLastFrame));
					mLastFrame = now;
				};
			}
			while (mSetupState != mSetupSteps.begin())
			{
				--mSetupState;
				std::optional<SignalSlot::TaskHandle> finalize = std::move(mSetupState->second);
				if (finalize)
					return finalize;
			}
			return {};
		}

		bool FrameLoop::empty()
		{
			return mTaskQueue.empty() && mSetupState == mSetupSteps.begin();
		}

		void FrameLoop::addSetupSteps(std::optional<SignalSlot::TaskHandle>&& init, std::optional<SignalSlot::TaskHandle>&& finalize)
		{
			bool isItEnd = mSetupState == mSetupSteps.end();
			if (init && finalize)
			{
				std::promise<bool> p;
				std::future<bool> f = p.get_future();
				mSetupSteps.emplace_back(
					[init{ std::move(*init) }, p{ std::move(p) }]() mutable {
						SignalSlot::TaskState state;
						try { 
							state = init();
							assert(state == SignalSlot::SUCCESS || state == SignalSlot::FAILURE);
							p.set_value(state == SignalSlot::SUCCESS);
						}
						catch (std::exception &)
						{
							p.set_value(false);
							throw;
						}
						return state;
					},
					[finalize{ std::move(*finalize) }, f{ std::move(f) }]() mutable {
						if (f.get())
							return finalize();
						return SignalSlot::SUCCESS;
					}
				);
			}
			else
			{
				mSetupSteps.emplace_back(std::move(init), std::move(finalize));
			}
			if (isItEnd)
			{
				mSetupState = std::prev(mSetupState);
			}
		}

	}
}