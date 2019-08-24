#include "../serverlib.h"

#if ENABLE_THREADING

#include "serverappinstance.h"

namespace Engine
{
	namespace Server
	{
		size_t ServerAppInstance::sInstanceCounter = 0;

		ServerAppInstance::~ServerAppInstance()
		{
			if (mApplication)
				mApplication->shutdown();
		}

		const char* ServerAppInstance::key() const
		{
			return mName.c_str();
		}

		/*ValueType ServerAppInstance::toValueType() const
		{
			return ValueType(Scripting::LuaThread(mApplication->lua_state()));
		}*/
	}
}

#endif