#include "serverlib.h"

#if ENABLE_THREADING

#include "serverinstance.h"

namespace Engine
{
	namespace Server
	{
		size_t ServerInstance::sInstanceCounter = 0;

		const char* ServerInstance::key() const
		{
			return mName.c_str();
		}

	}
}

#endif