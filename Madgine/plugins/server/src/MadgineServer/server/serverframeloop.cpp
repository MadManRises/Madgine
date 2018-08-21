#include "../serverlib.h"

#include "serverframeloop.h"


extern "C" DLL_EXPORT Engine::Core::FrameLoop *frameloop() { return new Engine::Server::ServerFrameLoop; }

namespace Engine
{
	namespace Server
	{

		int ServerFrameLoop::go()
		{

			mTimer.reset();
			return FrameLoop::go();
		}

		bool ServerFrameLoop::singleFrame()
		{
			std::chrono::microseconds timeSinceLastFrame = mTimer.elapsed_us();
			mTimer.start();
			return sendFrameStarted(timeSinceLastFrame) && sendFrameRenderingQueued(timeSinceLastFrame) && sendFrameEnded(timeSinceLastFrame);
		}


	}
}
