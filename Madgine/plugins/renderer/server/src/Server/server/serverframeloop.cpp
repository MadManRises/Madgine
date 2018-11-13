#include "../serverlib.h"

#include "serverframeloop.h"


extern "C" DLL_EXPORT Engine::Core::FrameLoop *frameloop() { return new Engine::Server::ServerFrameLoop; }

namespace Engine
{
	namespace Server
	{

	}
}
