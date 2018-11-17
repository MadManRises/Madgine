#include "../../interfaceslib.h"
#include "luatableinstance.h"

#include "argumentlist.h"
#include "intluatableinstance.h"
#include "stringluatableinstance.h"
#include "../types/scopebase.h"

#include "../../generic/valuetype.h"

extern "C"
{
#include "../../lua/lua.h"
#include "../../lua/lauxlib.h"
}

namespace Engine
{
	namespace Scripting
	{

		LuaTableInstance::LuaTableInstance(lua_State* state, const std::shared_ptr<LuaTableInstance>& parent) :
			mParent(parent),
			mState(state)
		{
		}

		void LuaTableInstance::setMetatable(const std::string& metatable)
		{
			push();
			luaL_setmetatable(mState, metatable.c_str());
			lua_pop(mState, 1);
		}


		void LuaTableInstance::setValue(const std::string& name, const ValueType& value)
		{
			push();
			value.push(mState);
			lua_setfield(mState, -2, name.c_str());
			lua_pop(mState, 1);
		}

		ValueType LuaTableInstance::getValue(const std::string& name) const
		{
			push();
			lua_getfield(mState, -1, name.c_str());
			ValueType result = ValueType::fromStack(mState, -1);
			lua_pop(mState, 2);
			return result;
		}

		void LuaTableInstance::setMetatable(const LuaTable& metatable)
		{
			push();
			metatable.push();
			lua_setmetatable(mState, -2);
			lua_pop(mState, 1);
		}

		std::shared_ptr<LuaTableInstance> LuaTableInstance::createTable(lua_State* state,
		                                                                const std::shared_ptr<LuaTableInstance>& ptr)
		{
			push(state ? state : mState);
			lua_newtable(state ? state : mState);
			int index = luaL_ref(state ? state : mState, -2);
			lua_pop(state ? state : mState, 1);
			return std::make_shared<IntLuaTableInstance>(state ? state : mState, index, ptr);
		}

		std::shared_ptr<LuaTableInstance> LuaTableInstance::createTable(const std::string& name,
		                                                                const std::shared_ptr<LuaTableInstance>& ptr)
		{
			push(mState);
			lua_newtable(mState);
			lua_setfield(mState, -2, name.c_str());
			lua_pop(mState, 1);
			return std::make_shared<StringLuaTableInstance>(mState, name, ptr);
		}

		std::shared_ptr<LuaTableInstance> LuaTableInstance::registerTable(lua_State* state, int index,
		                                                                  const std::shared_ptr<LuaTableInstance>& ptr)
		{
			push(state);
			if (index < 0 && index > -100)
				--index;
			lua_pushvalue(state ? state : mState, index);
			int regIndex = luaL_ref(state ? state : mState, -2);
			lua_pop(state ? state : mState, 1);
			return std::make_shared<IntLuaTableInstance>(state ? state : mState, regIndex, ptr);
		}

		void LuaTableInstance::setLightUserdata(const std::string& name, void* userdata)
		{
			push();
			lua_pushlightuserdata(mState, userdata);
			lua_setfield(mState, -2, name.c_str());
			lua_pop(mState, 1);
		}

		bool LuaTableInstance::hasFunction(const std::string& name) const
		{
			push();
			lua_getfield(mState, -1, name.c_str());
			bool result = lua_isfunction(mState, -1);
			lua_pop(mState, 2);
			return result;
		}

		ArgumentList LuaTableInstance::callMethod(const std::string& name, const ArgumentList& args)
		{
			int level = lua_gettop(mState);

			push();

			lua_getfield(mState, -1, name.c_str());

			if (!lua_isfunction(mState, -1))
			{
				ValueType v = ValueType::fromStack(mState, -2);

				std::string object = v.is<ScopeBase*>() ? v.as<ScopeBase*>()->getIdentifier() : "<anonymous Table>";

				lua_pop(mState, 2);

				throw ScriptingException(Database::Exceptions::unknownMethod(name, object));
			}

			lua_insert(mState, -2);

			args.pushToStack(mState);

			switch (lua_pcall(mState, static_cast<int>(args.size()) + 1, LUA_MULTRET, 0))
			{
			case 0:
				break;
			case LUA_ERRRUN:
				{
					int iType = lua_type(mState, -1);
					const char* msg = "unknown Error";
					if (iType == LUA_TSTRING)
					{
						msg = lua_tostring(mState, -1);
					}
					else
					{
						lua_pop(mState, 1);
					}
					throw ScriptingException("Runtime Error: "s + msg);
				}
			case LUA_ERRMEM:
				throw ScriptingException("Lua Out-Of-Memory!");
			default:
				throw 0;
			}
			int nresults = lua_gettop(mState) - level;

			return ArgumentList(mState, nresults);
		}

		lua_State* LuaTableInstance::state() const
		{
			return mState;
		}

	}
}
