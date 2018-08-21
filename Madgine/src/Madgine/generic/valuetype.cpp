#include "../interfaceslib.h"
#include "valuetype.h"

#include "../scripting/datatypes/argumentlist.h"

#include "../scripting/types/luastate.h"

#include "../scripting/types/scopebase.h"
#include "../scripting/types/apihelper.h"

extern "C"
{
#include <lua/lua.h>                                /* Always include this when calling Lua */
#include <lua/lauxlib.h>                            /* Always include this when calling Lua */
}

namespace Engine
{
	std::string ValueType::toString() const
	{
		switch (type())
		{
		case Type::BoolValue:
			return std::get<bool>(mUnion) ? "true" : "false";
		case Type::StringValue:
			return std::string("\"") + std::get<std::string>(mUnion) + "\"";
		case Type::IntValue:
			return std::to_string(std::get<int>(mUnion));
		case Type::UIntValue:
			return std::to_string(std::get<size_t>(mUnion));
		case Type::NullValue:
			return "NULL";
		case Type::EndOfListValue:
			return "EOL";
		case Type::ScopeValue:
			return std::get<Scripting::ScopeBase*>(mUnion)->getIdentifier();
		case Type::InvScopePtrValue:
			return std::to_string(reinterpret_cast<uintptr_t>(std::get<InvScopePtr>(mUnion).validate()));
		case Type::FloatValue:
			return std::to_string(std::get<float>(mUnion));
		case Type::Vector2Value:
			return std::string("[") + std::to_string(std::get<Vector2>(mUnion).x) + ", " + std::to_string(std::get<Vector2>(mUnion).y);
		case Type::Vector3Value:
			return std::string("[") + std::to_string(std::get<Vector3>(mUnion).x) + ", " + std::
				to_string(std::get<Vector3>(mUnion).y) + ", " + std::to_string(std::get<Vector3>(mUnion).z) + "]";
		case Type::Vector4Value:
			return std::string("[") +
				std::to_string(std::get<std::array<float, 4>>(mUnion)[0]) + ", " +
				std::to_string(std::get<std::array<float, 4>>(mUnion)[1]) + ", " +
				std::to_string(std::get<std::array<float, 4>>(mUnion)[2]) + ", " +
				std::to_string(std::get<std::array<float, 4>>(mUnion)[3]) + "]";
		default:
			MADGINE_THROW(Scripting::ScriptingException("Unknown Type!"));
		}
	}

	std::string ValueType::getTypeString() const
	{
		return getTypeString(type());
	}

	std::string ValueType::getTypeString(Type type)
	{
		switch (type)
		{
		case Type::BoolValue:
			return "Bool";
		case Type::EndOfListValue:
			return "EOL-Type";
		case Type::FloatValue:
			return "Float";
		case Type::IntValue:
			return "Integer";
		case Type::UIntValue:
			return "Unsigned Integer";
		case Type::InvScopePtrValue:
			return "Invalid Scope";
		case Type::NullValue:
			return "Null-Type";
		case Type::ScopeValue:
			return "Scope";
		case Type::StringValue:
			return "String";
		case Type::Vector2Value:
			return "Vector2";
		case Type::Vector3Value:
			return "Vector3";
		case Type::Vector4Value:
			return "Vector4";
		case Type::ApiMethodValue:
			return "Method";
		default:
			throw 0;
		}
	}


	ValueType ValueType::fromStack(lua_State* state, int index)
	{
		switch (lua_type(state, index))
		{
		case LUA_TBOOLEAN:
			return ValueType(lua_toboolean(state, index));
		case LUA_TNUMBER:
			return ValueType(static_cast<float>(lua_tonumber(state, index)));
		case LUA_TNIL:
			return ValueType();
		case LUA_TTABLE:
			{
				lua_pushliteral(state, "___scope___");

				if (index > -100 && index < 0)
					--index;

				ValueType result;
				if (lua_rawget(state, index) == LUA_TLIGHTUSERDATA)
				{
					Scripting::ScopeBase* scope = static_cast<Scripting::ScopeBase*>(lua_touserdata(state, -1));
					assert(scope);
					result = scope;
				}
				else
				{
					result = Scripting::LuaState::getGlobalTable(state).registerTable(state, index);
				}
				lua_pop(state, 1);

				return result;
			}
		case LUA_TSTRING:
			return ValueType(lua_tostring(state, index));
		case LUA_TUSERDATA:
			{
				Scripting::APIHelper::Userdata *userdata = static_cast<Scripting::APIHelper::Userdata*>(lua_touserdata(state, -1));
				if (std::holds_alternative<std::shared_ptr<KeyValueIterator>>(*userdata))
				{
					return ValueType(std::get<std::shared_ptr<KeyValueIterator>>(*userdata));
				}
				else if(std::holds_alternative<Scripting::ApiMethod>(*userdata))
				{
					return ValueType(std::get<Scripting::ApiMethod>(*userdata));
				}
			}			
		default:
			throw 0;
		}
	}

	int ValueType::push(lua_State* state) const
	{
		switch (type())
		{
		case Type::BoolValue:
			lua_pushboolean(state, as<bool>());
			return 1;
		case Type::FloatValue:
			lua_pushnumber(state, as<float>());
			return 1;
		case Type::IntValue:
			lua_pushinteger(state, as<int>());
			return 1;
		case Type::UIntValue:
			lua_pushinteger(state, as<size_t>());
			return 1;
		case Type::NullValue:
			lua_pushnil(state);
			return 1;
		case Type::ScopeValue:
			as<Scripting::ScopeBase*>()->push();
			return 1;
		case Type::StringValue:
			lua_pushstring(state, as<std::string>().c_str());
			return 1;
		case Type::ApiMethodValue:
			pushUserdata(state, as<Scripting::ApiMethod>());
			lua_pushcclosure(state, apiMethodCaller, 1);
			return 1;
		case Type::KeyValueIteratorValue:
			pushUserdata(state, as<std::shared_ptr<KeyValueIterator>>());
			return 1;
		case Type::LuaTableValue:
			as<Scripting::LuaTable>().push(state);
			return 1;
		default:
			throw 0;
		}
	}


	Scripting::APIHelper::Userdata* ValueType::pushUserdata(lua_State* state, const Scripting::APIHelper::Userdata& data) const
	{
		Scripting::APIHelper::Userdata *userdata = new (lua_newuserdata(state, sizeof(Scripting::APIHelper::Userdata))) Scripting::APIHelper::Userdata(data);

		luaL_getmetatable(state, "Interfaces.kvUserdataMetatable");

		lua_setmetatable(state, -2);

		return userdata;
	}

	int ValueType::apiMethodCaller(lua_State* state)
	{
		int n = lua_gettop(state);
		if (n < 1)
			luaL_error(state, "Memberfunctions need at least 1 argument!");
		Scripting::ArgumentList args(state, n - 1);
		Scripting::ApiMethod method = std::get<Scripting::ApiMethod>(*static_cast<Scripting::APIHelper::Userdata*>(lua_touserdata(state, lua_upvalueindex(1))));
		ValueType v = fromStack(state, -1);
		lua_pop(state, 1);
		if (!v.is<Scripting::ScopeBase*>())
			luaL_error(state, "First arguments has to be a Scope-type!");
		Scripting::ScopeBase* scope = v.as<Scripting::ScopeBase*>();		
		return (*method)(scope, args).push(state);
	}
}


std::ostream& operator <<(std::ostream& stream,
                          const Engine::ValueType& v)
{
	return stream << v.toString();
}
