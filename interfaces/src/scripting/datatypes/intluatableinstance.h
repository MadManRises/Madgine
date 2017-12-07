#pragma once

#include "luatableinstance.h"

namespace Engine
{
	namespace Scripting
	{
		
		class IntLuaTableInstance : public LuaTableInstance
		{
		public:
			IntLuaTableInstance(lua_State* state, int index, const std::shared_ptr<LuaTableInstance>& parent = {});
			virtual ~IntLuaTableInstance();
			void push(lua_State* state = nullptr) const override;
		private:
			int mIndex;
		};
	}
}