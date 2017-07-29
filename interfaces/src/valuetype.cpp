#include "interfaceslib.h"
#include "valuetype.h"

#include "scripting/types/api.h"

#include "serialize/serializeexception.h"

#include "scripting/datatypes/argumentlist.h"

#include "scripting/types/globalscopebase.h"

extern "C" {
#include <lua.h>                                /* Always include this when calling Lua */
#include <lauxlib.h>                            /* Always include this when calling Lua */
#include <lualib.h>                             /* Always include this when calling Lua */
}

namespace Engine {

	void Serialize::UnitHelper<ValueType, false>::applyMap(const std::map<size_t, SerializableUnitBase*> &map, ValueType &item)
	{
		/*if (item.isInvPtr()) {
			auto it = map.find(static_cast<size_t>(item.asInvPtr()));
			if (it != map.end()) {
				Scripting::ScopeBase *scope = dynamic_cast<Scripting::ScopeBase*>(it->second);
				if (scope) {
					item = scope;
				}
				else {
					LOG_WARNING("Non-Scope InvScopePtr in ValueType!");
				}
			}
			else {
				LOG_WARNING("Unmapped InvScopePtr!");
			}
		}*/
	}

std::string ValueType::toString() const
{
	switch (type()) {
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
	case Type::FloatValue:
		return std::to_string(std::get<bool>(mUnion));
	case Type::Vector3Value:
		return std::string("[") + std::to_string(std::get<Vector3>(mUnion).x) + ", " + std::to_string(std::get<Vector3>(mUnion).y) + ", " + std::to_string(std::get<Vector3>(mUnion).z) + "]";
	default:
		MADGINE_THROW(Scripting::ScriptingException("Unknown Type!"));
	}


}


ValueType ValueType::fromStack(lua_State * state, int index)
{
	switch (lua_type(state, index)) {
	case LUA_TBOOLEAN:
		return ValueType(lua_toboolean(state, index));
	case LUA_TNUMBER:
		return ValueType(static_cast<float>(lua_tonumber(state, index)));
	case LUA_TNIL:
		return ValueType();
	case LUA_TTABLE: {
		lua_pushliteral(state, "___scope___");

		if (index > -100 && index < 0)
			--index;

		ValueType result;
		if (lua_rawget(state, index) == LUA_TLIGHTUSERDATA) {

			Scripting::ScopeBase *scope = static_cast<Scripting::ScopeBase*>(lua_touserdata(state, -1));
			assert(scope);
			result = scope;
		}
		else {
			result = Scripting::GlobalScopeBase::getSingleton().table().registerTable(state, index);
		}
		lua_pop(state, 1);

		return result;
	}
	case LUA_TSTRING:
		return ValueType(lua_tostring(state, index));
	case LUA_TUSERDATA:
		return ValueType(*static_cast<std::shared_ptr<Scripting::KeyValueIterator>*>(lua_touserdata(state, -1)));
	default:
		throw 0;
	}
}

int ValueType::push(lua_State * state) const
{
	switch (type()) {
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
		lua_pushlightuserdata(state, as<Scripting::ApiMethod>());
		lua_pushcclosure(state, apiMethodCaller, 1);
		return 1;
	case Type::KeyValueIteratorValue:
	{
		std::shared_ptr<Scripting::KeyValueIterator> *itP = static_cast<std::shared_ptr<Scripting::KeyValueIterator>*>(lua_newuserdata(state, sizeof(std::shared_ptr<Scripting::KeyValueIterator>)));
		new (itP) std::shared_ptr<Scripting::KeyValueIterator>(as<std::shared_ptr<Scripting::KeyValueIterator>>());

		luaL_getmetatable(state, "Interfaces.kvIteratorMetatable");

		lua_setmetatable(state, -2);

		return 1;
	}
	case Type::LuaTableValue:
		as<Scripting::LuaTable>().push(state);
		return 1;
	default:
		throw 0;
	}
}


int ValueType::apiMethodCaller(lua_State *state)
{
	int n = lua_gettop(state);
	Engine::Scripting::ArgumentList args(state, n-1);
	Scripting::ApiMethod method = reinterpret_cast<Scripting::ApiMethod>(lua_touserdata(state, lua_upvalueindex(1)));
	Engine::Scripting::ScopeBase *scope = fromStack(state, -1).as<Scripting::ScopeBase*>();
	lua_pop(state, 1);
	return (*method)(scope, args).push(state);
}

}




std::ostream &operator <<(std::ostream &stream,
	const Engine::ValueType &v)
{
	return stream << v.toString();
}
