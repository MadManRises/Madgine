#include "../baselib.h"

#include "frameloop.h"
#include "framelistener.h"

#include "Interfaces/signalslot/connectionmanager.h"

#include "Interfaces/debug/profiler/profiler.h"

namespace Engine
{
	namespace Core
	{
		FrameLoop::FrameLoop() :
			mTimeBank(0),
		mShutdown(true)
		{
		}

		int FrameLoop::go()
		{			
			startLoop();
			auto now = std::chrono::high_resolution_clock::now();
			auto oldNow = now;
			while (!mShutdown && singleFrame(std::chrono::duration_cast<std::chrono::microseconds>(now - oldNow))) {
				std::this_thread::yield();
				oldNow = now;
				now = std::chrono::high_resolution_clock::now();
			}
			return 0;
		}

		bool FrameLoop::init()
		{
			return true;
		}

		void FrameLoop::finalize()
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
			mShutdown = true;
		}

		bool FrameLoop::isShutdown()
		{
			return mShutdown;
		}

		void FrameLoop::startLoop()
		{
			mShutdown = false;
		}

		bool FrameLoop::sendFrameStarted(std::chrono::microseconds timeSinceLastFrame)
		{
			PROFILE();
			SignalSlot::ConnectionManager::getSingleton().update();
			
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


	}
}