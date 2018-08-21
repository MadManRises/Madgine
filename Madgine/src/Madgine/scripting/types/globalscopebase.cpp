#include "../../baselib.h"
#include "globalscopebase.h"

#include "../types/luastate.h"




namespace Engine
{

	namespace Scripting
	{


		GlobalScopeBase::GlobalScopeBase(LuaState &state) :
			ScopeBase(state.createThread(this)),
			mState(state)
		{
			assert(table());

			table().setMetatable("Interfaces.GlobalScope");
		}


		void GlobalScopeBase::executeString(const std::string& cmd)
		{
			mState.executeString(table().state(), cmd);
		}

		lua_State* GlobalScopeBase::lua_state() const
		{
			return table().state();
		}

		
	}
}
