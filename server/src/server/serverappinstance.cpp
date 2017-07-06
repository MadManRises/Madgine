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
		
		const std::string &Engine::Server::ServerAppInstance::key() const
		{
			return mName;
		}

		int ServerAppInstance::push(lua_State * state) const
		{
			return mParser.pushThread(state, mState.first);
		}

	}

	namespace Scripting {
		int LuaHelper<Server::ServerAppInstance>::push(lua_State * state, const Server::ServerAppInstance & it)
		{
			return it.push(state);
		}
	}

}


