#pragma once

#include "scopebase.h"

namespace Engine
{
	namespace Scripting
	{
		class INTERFACES_EXPORT GlobalScopeBase : public ScopeBase
		{
		public:
			GlobalScopeBase(LuaState &state);

			void executeString(const std::string& cmd);

			lua_State* lua_state() const;

			using ScopeBase::table;			

		private:
			LuaState &mState;
		};
	}
}