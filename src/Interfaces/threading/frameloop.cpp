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
			mLastFrame(std::chrono::high_resolution_clock::now())
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

		void FrameLoop::queueTeardown(SignalSlot::TaskHandle &&task)
		{
			mTeardownQueue.emplace_front(std::move(task));
		}

		std::optional<SignalSlot::TaskHandle> FrameLoop::fetch()
		{
			if (!mTaskQueue.empty())
			{
				SignalSlot::TaskHandle task = std::move(mTaskQueue.front());
				mTaskQueue.pop_front();
				return task;
			}
			else
			{
				if (mRunning)
				{
					return [this]() {
						auto now = std::chrono::high_resolution_clock::now();
						mRunning &= singleFrame(std::chrono::duration_cast<std::chrono::microseconds>(now - mLastFrame));
						mLastFrame = now;
					};
				}
				else
				{
					if (!mTeardownQueue.empty())
					{
						SignalSlot::TaskHandle task = std::move(mTeardownQueue.front());
						mTeardownQueue.pop_front();
						return task;
					}
					else
					{
						return {};
					}
				}
			}			
			
		}

		bool FrameLoop::empty()
		{
			return mTaskQueue.empty();
		}

	}
}