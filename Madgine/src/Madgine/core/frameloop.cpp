#include "../baselib.h"

#include "frameloop.h"
#include "framelistener.h"

namespace Engine
{
	namespace Core
	{
		FrameLoop::FrameLoop() :
			mTimeBank(0.0f)
		{
		}

		int FrameLoop::go()
		{
			while (singleFrame()) std::this_thread::yield();
			return 0;
		}

		bool FrameLoop::preInit()
		{
			return true;
		}

		bool FrameLoop::init()
		{
			return true;
		}

		void FrameLoop::finalize()
		{
		}

		bool FrameLoop::singleFrame()
		{
			return sendFrameStarted(0) && sendFrameRenderingQueued(0) && sendFrameEnded(0);
		}

		void FrameLoop::addFrameListener(FrameListener* listener)
		{
			mListeners.push_back(listener);
		}

		void FrameLoop::removeFrameListener(FrameListener* listener)
		{
			mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
		}

		bool FrameLoop::sendFrameStarted(float timeSinceLastFrame)
		{
			bool result = true;
			for (Core::FrameListener* listener : mListeners)
				result &= listener->frameStarted(timeSinceLastFrame);
			return result;
		}

		bool FrameLoop::sendFrameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context)
		{
			bool result = true;
			for (Core::FrameListener* listener : mListeners)
				result &= listener->frameRenderingQueued(timeSinceLastFrame, context);
			
			mTimeBank += timeSinceLastFrame;

			while (mTimeBank >= FIXED_TIMESTEP && result)
			{
				result &= sendFrameFixedUpdate(FIXED_TIMESTEP);
				mTimeBank -= FIXED_TIMESTEP;
			}

			return result;
		}

		bool FrameLoop::sendFrameFixedUpdate(float timeSinceLastFrame, Scene::ContextMask context)
		{
			bool result = true;
			for (Core::FrameListener* listener : mListeners)
				result &= listener->frameFixedUpdate(timeSinceLastFrame, context);
			return result;
		}

		float FrameLoop::fixedRemainder() const
		{
			return FIXED_TIMESTEP - mTimeBank;
		}

		bool FrameLoop::sendFrameEnded(float timeSinceLastFrame)
		{
			bool result = true;
			for (Core::FrameListener* listener : mListeners)
				result &= listener->frameEnded(timeSinceLastFrame);
			return result;
		}


	}
}