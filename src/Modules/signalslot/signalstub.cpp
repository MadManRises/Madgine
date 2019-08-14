#include "../moduleslib.h"

#include "signalstub.h"

namespace Engine
{
	namespace SignalSlot
	{
		std::mutex sSignalConnectMutex;
	}
}