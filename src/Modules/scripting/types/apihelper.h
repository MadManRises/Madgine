#pragma once

#include "../../keyvalue/keyvalueiterate.h"

namespace Engine
{
	namespace Scripting
	{
		class MODULES_EXPORT APIHelper
		{
		public:
			static void createMetatables(lua_State* state);

			static void pop(lua_State* state, int n);
			static int stackSize(lua_State* state);
			static int error(lua_State* state, const std::string& msg);


			using Userdata = std::variant<
                            KeyValueVirtualIterator,
				ApiMethod
			>;

		private:
			static int lua_gc(lua_State* state);
			static int lua_toString(lua_State* state);

			static const luaL_Reg sUserdataMetafunctions[];

			
		};
	} // namespace Scripting
} // namespace Core
