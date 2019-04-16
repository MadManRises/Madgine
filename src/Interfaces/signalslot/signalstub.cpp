#include "../interfaceslib.h"

#include "signalstub.h"

namespace Engine
{
	namespace SignalSlot
	{
		std::mutex sSignalConnectMutex;
	}
}