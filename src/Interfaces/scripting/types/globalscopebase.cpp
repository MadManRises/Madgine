#include "../../interfaceslib.h"
#include "globalscopebase.h"

#include "../types/luastate.h"

#include "../datatypes/luathread.h"


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
			std::lock_guard guard(*table().thread());
			mState.executeString(table().thread()->state(), cmd);
		}

		LuaThread* GlobalScopeBase::luaThread() const
		{
			return table().thread();
		}

		
	}
}
