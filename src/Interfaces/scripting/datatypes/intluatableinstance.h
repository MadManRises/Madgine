#pragma once

#include "luatableinstance.h"

namespace Engine
{
	namespace Scripting
	{
		
		class IntLuaTableInstance : public LuaTableInstance
		{
		public:
			IntLuaTableInstance(LuaThread* thread, int index, const std::shared_ptr<LuaTableInstance>& parent = {});			
			virtual ~IntLuaTableInstance();
			void push(lua_State* state) const override;
		private:
			int mIndex;
		};
	}
}