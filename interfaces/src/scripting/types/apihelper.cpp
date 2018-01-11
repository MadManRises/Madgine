#include "interfaceslib.h"

#include "apihelper.h"
#include "generic/keyvalueiterate.h"

extern "C"
{
#include <lua.h>                                /* Always include this when calling Lua */
#include <lauxlib.h>                            /* Always include this when calling Lua */
}

namespace Engine
{
	namespace Scripting
	{
		const luaL_Reg APIHelper::sIteratorMetafunctions[] =
		{
			{"__gc", &lua_gc},
			{"__tostring", &lua_toString},
			//{ "__pairs", &lua_pairs },
			//{"__newindex", &lua_newindex}
			{nullptr, nullptr}
		};

		int APIHelper::lua_gc(lua_State* state)
		{
			std::shared_ptr<KeyValueIterator>* itP = static_cast<std::shared_ptr<KeyValueIterator>*>(lua_touserdata(state, -1));
			itP->~shared_ptr();
			lua_pop(state, 1);
			return 0;
		}

		int APIHelper::lua_toString(lua_State* state)
		{
			std::shared_ptr<KeyValueIterator>* itP = static_cast<std::shared_ptr<KeyValueIterator>*>(lua_touserdata(state, -1));
			lua_pop(state, 1);
			lua_pushstring(state, (*itP)->key().c_str());
			return 1;
		}

		void APIHelper::createMetatables(lua_State* state)
		{
			luaL_newmetatable(state, "Interfaces.kvIteratorMetatable");

			luaL_setfuncs(state, sIteratorMetafunctions, 0);

			lua_pop(state, 1);
		}

		/*int LuaHelper<KeyValueIterator*>::push(lua_State * state, KeyValueIterator * it)
		{
			
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
				lua_pushboolean(state, v.as<bool>());
				break;
			case ValueType::Type::FloatValue:
				lua_pushnumber(state, v.as<float>());
				break;
			case ValueType::Type::IntValue:
				lua_pushinteger(state, v.as<int>());
				break;
			case ValueType::Type::UIntValue:
				lua_pushinteger(state, v.as<size_t>());
				break;
			case ValueType::Type::NullValue:
				lua_pushnil(state);
				break;
			case ValueType::Type::ScopeValue:
				v.as<ScopeBase*>()->push();
				break;
			case ValueType::Type::StringValue:
				lua_pushstring(state, v.as<std::string>().c_str());
				break;
			default:
				throw 0;
			}
			return 1;
		}*/

		void APIHelper::pop(lua_State* state, int n)
		{
			lua_pop(state, n);
		}

		/*
		ValueType LuaHelper<ValueType>::convert(lua_State * state, int index)
		{
			switch (lua_type(state, index)) {

			}
			
		}*/

		int APIHelper::stackSize(lua_State* state)
		{
			return lua_gettop(state);
		}

		int APIHelper::error(lua_State* state, const std::string& msg)
		{
			return luaL_error(state, msg.c_str());
		}

		/*int LuaRefHelper<Mapper>::push(lua_State * state, ScopeBase * ref, const Mapper & mapper)
		{
			return (*mapper.mPush)(state, ref);
		}*/
	}
}
