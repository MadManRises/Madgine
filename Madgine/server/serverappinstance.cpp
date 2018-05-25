#include "../serverlib.h"
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
			if (mThread.joinable())
				mThread.join();
		}

		const char* ServerAppInstance::key() const
		{
			return mName.c_str();
		}

		ValueType ServerAppInstance::toValueType() const
		{
			return ValueType(Scripting::LuaThread(mApplication->lua_state()));
		}
	}
}
