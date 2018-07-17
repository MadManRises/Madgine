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
			float timeSinceLastFrame = mTimer.elapsed_us() / 1000000.0f;
			mTimer.start();
			return sendFrameStarted(timeSinceLastFrame) && sendFrameRenderingQueued(timeSinceLastFrame) && sendFrameEnded(timeSinceLastFrame);
		}


	}
}
