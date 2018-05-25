#pragma once

namespace Engine
{
	namespace Scripting
	{
		class INTERFACES_EXPORT APIHelper
		{
		public:
			static void createMetatables(lua_State* state);

			static void pop(lua_State* state, int n);
			static int stackSize(lua_State* state);
			static int error(lua_State* state, const std::string& msg);

		private:
			static int lua_gc(lua_State* state);
			static int lua_toString(lua_State* state);

			static const luaL_Reg sIteratorMetafunctions[];
		};
	} // namespace Scripting
} // namespace Core
