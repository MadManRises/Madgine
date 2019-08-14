#pragma once


namespace Engine
{
	namespace Scripting
	{
		MODULES_EXPORT ArgumentList popFromStack(lua_State *state, int count);
		MODULES_EXPORT void pushToStack(lua_State *state, const ArgumentList &args);
	}
}
