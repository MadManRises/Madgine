#include "interfaceslib.h"

#include "apihelper.h"

#include "globalscopebase.h"

#include <iostream>

#include "api.h"

extern "C" {
#include <lua.h>                                /* Always include this when calling Lua */
#include <lauxlib.h>                            /* Always include this when calling Lua */
#include <lualib.h>                             /* Always include this when calling Lua */
}

namespace Engine {
	namespace Scripting {

		const luaL_Reg APIHelper::sIteratorMetafunctions[] =
		{
			{ "__gc", &lua_gc },
			{ "__tostring", &lua_toString },
			//{ "__pairs", &lua_pairs },
			//{"__newindex", &lua_newindex}
			{ NULL, NULL }
		};

		int APIHelper::lua_gc(lua_State *state) {
			KeyValueIterator *itP = APIHelper::to<KeyValueIterator*>(state, -1);
			delete itP;
			lua_pop(state, 1);
			return 0;
		}

		int APIHelper::lua_toString(lua_State * state)
		{
			KeyValueIterator *itP = APIHelper::to<KeyValueIterator*>(state, -1);
			lua_pop(state, 1);
			lua_pushstring(state, itP->key().c_str());
			return 1;
		}

		void APIHelper::createMetatables(lua_State *state) {
			luaL_newmetatable(state, "Interfaces.kvIteratorMetatable");

			luaL_setfuncs(state, sIteratorMetafunctions, 0);

			lua_pop(state, 1);
		}

		int LuaHelper<ScopeBase*>::push(lua_State *state, ScopeBase *scope) {
			scope->push();
			return 1;
		}

		int LuaHelper<KeyValueIterator*>::push(lua_State * state, KeyValueIterator * it)
		{
			KeyValueIterator **itP = static_cast<KeyValueIterator**>(lua_newuserdata(state, sizeof(KeyValueIterator*)));
			*itP = it;

			luaL_getmetatable(state, "Interfaces.kvIteratorMetatable");

			lua_setmetatable(state, -2);

			return 1;
		}

		KeyValueIterator *LuaHelper<KeyValueIterator*>::convert(lua_State * state, int index)
		{
			KeyValueIterator **itP = static_cast<KeyValueIterator**>(lua_touserdata(state, index));
			return *itP;
		}

		int LuaHelper<int(*)(lua_State*)>::push(lua_State * state, int(*f)(lua_State *))
		{
			lua_pushcfunction(state, f);
			return 1;
		}

		int LuaHelper<ValueType>::push(lua_State * state, const ValueType &v)
		{
			switch (v.type()) {
			case ValueType::Type::BoolValue:
				lua_pushboolean(state, v.asBool());
				break;
			case ValueType::Type::FloatValue:
				lua_pushnumber(state, v.asFloat());
				break;
			case ValueType::Type::IntValue:
				lua_pushinteger(state, v.asInt());
				break;
			case ValueType::Type::NullValue:
				lua_pushnil(state);
				break;
			case ValueType::Type::ScopeValue:
				v.asScope()->push();
				break;
			case ValueType::Type::StringValue:
				lua_pushstring(state, v.asString().c_str());
				break;
			default:
				throw 0;
			}
			return 1;
		}

		void APIHelper::pop(lua_State * state, int n)
		{
			lua_pop(state, n);
		}

		ValueType LuaHelper<ValueType>::convert(lua_State * state, int index)
		{
			switch (lua_type(state, index)) {
			case LUA_TBOOLEAN:
				return ValueType(lua_toboolean(state, index));
			case LUA_TNUMBER:
				return ValueType(static_cast<float>(lua_tonumber(state, index)));
			case LUA_TNIL:
				return ValueType();
			case LUA_TTABLE:
				return ValueType(APIHelper::to<ScopeBase*>(state, index));
			case LUA_TSTRING:
				return ValueType(lua_tostring(state, index));
			default:
				throw 0;
			}
			
		}

		int APIHelper::stackSize(lua_State * state)
		{
			return lua_gettop(state);
		}

		ScopeBase *LuaHelper<ScopeBase*>::convert(lua_State * state, int index) {
			lua_pushliteral(state, "___scope___");

			if (index > -100 && index < 0)
				--index;

			lua_rawget(state, index);

			ScopeBase *scope = static_cast<ScopeBase*>(lua_touserdata(state, -1));

			lua_pop(state, 1);

			return scope;
		}

		int APIHelper::error(lua_State * state, const std::string & msg)
		{
			return luaL_error(state, msg.c_str());
		}

		int LuaHelper<Mapper>::push(lua_State * state, const Mapper & mapper)
		{
			return (*mapper.mPush)(state);
		}

	}
}