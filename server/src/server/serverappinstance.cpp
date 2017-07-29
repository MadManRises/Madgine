#include "serverlib.h"
#include "serverappinstance.h"
#include "scripting/parsing/scriptparser.h"


namespace Engine {
	namespace Server {

		size_t ServerAppInstance::sInstanceCounter = 0;

		ServerAppInstance::~ServerAppInstance()
		{
			if (mApplication)
				mApplication->shutdown();
			if (mThread.joinable())
				mThread.join();
		}
		
		const std::string &ServerAppInstance::key() const
		{
			return mName;
		}

		ValueType ServerAppInstance::toValueType() const
		{
			return ValueType(Scripting::LuaThread(mTable.state()));
		}

	}

}


