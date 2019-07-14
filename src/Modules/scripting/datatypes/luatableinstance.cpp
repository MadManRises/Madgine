#include "../../moduleslib.h"
#include "luatableinstance.h"

#include "intluatableinstance.h"
#include "stringluatableinstance.h"
#include "../../keyvalue/scopebase.h"

#include "../../keyvalue/valuetype.h"

extern "C"
{
#include "../../lua/lua.h"
#include "../../lua/lauxlib.h"
}

#include "../../keyvalue/metatable.h"

#include "luathread.h"

#include "argumentlist.h"

namespace Engine
{
	namespace Scripting
	{

		LuaTableInstance::LuaTableInstance(LuaThread* thread, const std::shared_ptr<LuaTableInstance>& parent) :
			mParent(parent),
			mThread(thread)
		{
			assert(mThread);
		}

		void LuaTableInstance::setMetatable(const std::string& metatable)
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			push(state);
			luaL_setmetatable(state, metatable.c_str());
			lua_pop(state, 1);
		}


		void LuaTableInstance::setValue(const std::string& name, const ValueType& value)
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			push(state);
			value.push(state);
			lua_setfield(state, -2, name.c_str());
			lua_pop(state, 1);
		}

		ValueType LuaTableInstance::getValue(const std::string& name) const
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			push(state);
			lua_getfield(state, -1, name.c_str());
			ValueType result = ValueType::fromStack(state, -1);
			lua_pop(state, 2);
			return result;
		}

		void LuaTableInstance::setMetatable(const LuaTable& metatable)
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			push(state);
			metatable.push(state);
			lua_setmetatable(state, -2);
			lua_pop(state, 1);
		}

		std::shared_ptr<LuaTableInstance> LuaTableInstance::createTable(LuaThread *thread,
		                                                                const std::shared_ptr<LuaTableInstance>& ptr)
		{
			if (!thread)
				thread = mThread;
			std::lock_guard guard(*thread);
			lua_State *state = thread->state();
			push(state);
			lua_newtable(state);
			int index = luaL_ref(state, -2);
			lua_pop(state, 1);
			return std::make_shared<IntLuaTableInstance>(thread, index, ptr);
		}

		std::shared_ptr<LuaTableInstance> LuaTableInstance::createTable(const std::string& name,
		                                                                const std::shared_ptr<LuaTableInstance>& ptr)
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			push(state);
			lua_newtable(state);
			lua_setfield(state, -2, name.c_str());
			lua_pop(state, 1);
			return std::make_shared<StringLuaTableInstance>(mThread, name, ptr);
		}

		std::shared_ptr<LuaTableInstance> LuaTableInstance::registerTable(lua_State* state, int index,
		                                                                  const std::shared_ptr<LuaTableInstance>& ptr)
		{
			push(state);
			if (index < 0 && index > -100)
				--index;
			lua_pushvalue(state, index);
			int regIndex = luaL_ref(state, -2);
			lua_pop(state, 1);
			return std::make_shared<IntLuaTableInstance>(mThread, regIndex, ptr);
		}

		void LuaTableInstance::setLightUserdata(const std::string& name, void* userdata)
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			push(state);
			lua_pushlightuserdata(state, userdata);
			lua_setfield(state, -2, name.c_str());
			lua_pop(state, 1);
		}

		bool LuaTableInstance::hasFunction(const std::string& name) const
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			push(state);
			lua_getfield(state, -1, name.c_str());
			bool result = lua_isfunction(state, -1);
			lua_pop(state, 2);
			return result;
		}

		ArgumentList LuaTableInstance::callMethod(const std::string& name, const ArgumentList& args)
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			int level = lua_gettop(state);

			push(state);

			lua_getfield(state, -1, name.c_str());

			if (!lua_isfunction(state, -1))
			{
				ValueType v = ValueType::fromStack(state, -2);

				std::string object = v.is<TypedScopePtr>() ? v.as<TypedScopePtr>().mType->mName : "<anonymous Table>";

				lua_pop(state, 2);

				throw ScriptingException(Database::Exceptions::unknownMethod(name, object));
			}

			lua_insert(state, -2);

			pushToStack(state, args);

			switch (lua_pcall(state, static_cast<int>(args.size()) + 1, LUA_MULTRET, 0))
			{
			case 0:
				break;
			case LUA_ERRRUN:
				{
					int iType = lua_type(state, -1);
					const char* msg = "unknown Error";
					if (iType == LUA_TSTRING)
					{
						msg = lua_tostring(state, -1);
					}
					else
					{
						lua_pop(state, 1);
					}
					throw ScriptingException("Runtime Error: "s + msg);
				}
			case LUA_ERRMEM:
				throw ScriptingException("Lua Out-Of-Memory!");
			default:
				throw 0;
			}
			int nresults = lua_gettop(state) - level;

			return popFromStack(state, nresults);
		}

		LuaThread* LuaTableInstance::thread() const
		{
			return mThread;
		}

	}
}
