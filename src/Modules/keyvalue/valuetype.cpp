#include "../moduleslib.h"
#include "valuetype.h"

#include "../scripting/types/luastate.h"

#include "../scripting/types/apihelper.h"
#include "scopebase.h"

#include "metatable.h"

#include "../scripting/datatypes/argumentlist.h"

extern "C" {
#include "../lua/lauxlib.h"
#include "../lua/lua.h"
}

namespace Engine {
std::string ValueType::toString() const
{
    switch (type()) {
    case Type::BoolValue:
        return std::get<bool>(mUnion) ? "true" : "false";
    case Type::StringValue:
        return "\""s + std::get<std::string>(mUnion) + "\"";
    case Type::IntValue:
        return std::to_string(std::get<int>(mUnion));
    case Type::UIntValue:
        return std::to_string(std::get<size_t>(mUnion));
    case Type::NullValue:
        return "NULL";
    case Type::ScopeValue:
        return std::get<TypedScopePtr>(mUnion).mType->mName;
    case Type::InvScopePtrValue:
        return std::to_string(reinterpret_cast<uintptr_t>(std::get<InvScopePtr>(mUnion).validate()));
    case Type::FloatValue:
        return std::to_string(std::get<float>(mUnion));
    case Type::Vector2Value:
        return "["s + std::to_string(std::get<Vector2>(mUnion).x) + ", " + std::to_string(std::get<Vector2>(mUnion).y);
    case Type::Vector3Value:
        return "["s + std::to_string(std::get<Vector3>(mUnion).x) + ", " + std::to_string(std::get<Vector3>(mUnion).y) + ", " + std::to_string(std::get<Vector3>(mUnion).z) + "]";
    case Type::Vector4Value:
        return "["s + std::to_string(std::get<Vector4>(mUnion)[0]) + ", " + std::to_string(std::get<Vector4>(mUnion)[1]) + ", " + std::to_string(std::get<Vector4>(mUnion)[2]) + ", " + std::to_string(std::get<Vector4>(mUnion)[3]) + "]";
    case Type::QuaternionValue:
        return "{"s + std::to_string(std::get<Quaternion>(mUnion).v.x) + ", " + std::to_string(std::get<Quaternion>(mUnion).v.y) + ", " + std::to_string(std::get<Quaternion>(mUnion).v.z) + ", " + std::to_string(std::get<Quaternion>(mUnion).w) + "}";
    case Type::ApiMethodValue:
        return "<method>";
    case Type::BoundApiMethodValue:
        return "<boundmethod>";
    default:
        throw Scripting::ScriptingException("Unknown Type!");
    }
}

std::string ValueType::getTypeString() const
{
    return getTypeString(type());
}

std::string ValueType::getTypeString(Type type)
{
    switch (type) {
    case Type::BoolValue:
        return "Bool";
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
    case Type::Matrix3Value:
        return "Matrix3x3";
    case Type::Vector2Value:
        return "Vector2";
    case Type::Vector3Value:
        return "Vector3";
    case Type::Vector4Value:
        return "Vector4";
    case Type::QuaternionValue:
        return "Quaternion";
    case Type::ApiMethodValue:
        return "Method";
    case Type::KeyValueVirtualIteratorValue:
        return "Iterator";
    case Type::BoundApiMethodValue:
        return "Bound Method";
    case Type::LuaTableValue:
        return "Lua Table";
	default:
        throw 0;
    }
}

ValueType ValueType::fromStack(lua_State *state, int index)
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

            if (index > -100 && index < 0)
                --index;

            lua_pushliteral(state, "___type___");

            if (lua_rawget(state, index) != LUA_TLIGHTUSERDATA)
                throw 0;

            ScopeBase *scope = static_cast<ScopeBase *>(lua_touserdata(state, -2));
            assert(scope);
            const MetaTable *type = static_cast<const MetaTable *>(lua_touserdata(state, -1));
            assert(type);
            result = TypedScopePtr { scope, type };

            lua_pop(state, 2);

        } else {
            result = Scripting::LuaState::getThread(state)->table().registerTable(state, index);
            lua_pop(state, 1);
        }

        return result;
    }
    case LUA_TSTRING:
        return ValueType(lua_tostring(state, index));
    case LUA_TUSERDATA: {
        Scripting::APIHelper::Userdata *userdata = static_cast<Scripting::APIHelper::Userdata *>(lua_touserdata(state, -1));
        if (std::holds_alternative<KeyValueVirtualIterator>(*userdata)) {
            return ValueType(std::get<KeyValueVirtualIterator>(*userdata));
        } else if (std::holds_alternative<ApiMethod>(*userdata)) {
            return ValueType(std::get<ApiMethod>(*userdata));
        }
    }
    default:
        throw 0;
    }
}

Scripting::APIHelper::Userdata *pushUserdata(lua_State *state, const Scripting::APIHelper::Userdata &data)
{
    Scripting::APIHelper::Userdata *userdata = new (lua_newuserdata(state, sizeof(Scripting::APIHelper::Userdata))) Scripting::APIHelper::Userdata(data);

    luaL_getmetatable(state, "Interfaces.kvUserdataMetatable");

    lua_setmetatable(state, -2);

    return userdata;
}

int ValueType::push(lua_State *state) const
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
        //as<Scripting::TypedScopePtr>().mScope->table().push(state);
        throw 0;
        return 1;
    case Type::StringValue:
        lua_pushstring(state, as<std::string>().c_str());
        return 1;
    case Type::ApiMethodValue:
        pushUserdata(state, as<ApiMethod>());
        lua_pushcclosure(state, apiMethodCaller, 1);
        return 1;
    case Type::KeyValueVirtualIteratorValue:
        pushUserdata(state, as<KeyValueVirtualIterator>());
        return 1;
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
    if (n < 1)
        luaL_error(state, "Memberfunctions need at least 1 argument!");
    ArgumentList args = Scripting::popFromStack(state, n - 1);
    ApiMethod method = std::get<ApiMethod>(*static_cast<Scripting::APIHelper::Userdata *>(lua_touserdata(state, lua_upvalueindex(1))));
    ValueType v = fromStack(state, -1);
    lua_pop(state, 1);
    if (!v.is<TypedScopePtr>())
        luaL_error(state, "First argument has to be a Scope-type!");
    TypedScopePtr scope = v.as<TypedScopePtr>();
    return method(scope, args).push(state);
}
}

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v)
{
    return stream << v.toString();
}
