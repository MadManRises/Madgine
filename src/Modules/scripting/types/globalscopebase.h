#pragma once

#include "scopeentity.h"

namespace Engine
{
	namespace Scripting
	{
		class MODULES_EXPORT GlobalScopeBase : public ScopeEntity<>
		{
		public:
			GlobalScopeBase(LuaState &state);

			void executeString(const std::string& cmd);

			LuaThread* luaThread() const;

			using ScopeEntity::table;			

		private:
			LuaState &mState;
		};
	}
}
