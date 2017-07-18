#include "interfaceslib.h"
#include "luatable.h"
#include "scripting/datatypes/argumentlist.h"

extern "C" {
#include <lua.h>                                /* Always include this when calling Lua */
#include <lauxlib.h>                            /* Always include this when calling Lua */
#include <lualib.h>                             /* Always include this when calling Lua */
}

namespace Engine {
	namespace Scripting {

		LuaTable::LuaTable()
		{
		}

		LuaTableInstance::LuaTableInstance(lua_State *state, const std::shared_ptr<LuaTableInstance> &parent) :
			mState(state),
			mParent(parent){

		}

		IntLuaTableInstance::~IntLuaTableInstance()
		{
			if (mIndex != LUA_REGISTRYINDEX){
				if (mParent) {
					mParent->push(mState);
					luaL_unref(mState, -1, mIndex);
					lua_pop(mState, 1);
				}
				else if (mIndex != LUA_RIDX_GLOBALS) {
					luaL_unref(mState, LUA_REGISTRYINDEX, mIndex);
				}
			}
		}

		StringLuaTableInstance::~StringLuaTableInstance()
		{
				if (mParent) {
					mParent->push(mState);
					lua_pushnil(mState);
					lua_setfield(mState, -2, mIndex.c_str());
					lua_pop(mState, 1);
				}
				else {
					lua_pushnil(mState);
					lua_setfield(mState, LUA_REGISTRYINDEX, mIndex.c_str());
				}
		}


		LuaTable LuaTable::global(lua_State * state)
		{
			return std::make_shared<IntLuaTableInstance>(state, LUA_RIDX_GLOBALS);
		}

		LuaTable LuaTable::registry(lua_State * state)
		{
			return std::make_shared<IntLuaTableInstance>(state, LUA_REGISTRYINDEX);
		}

		void LuaTableInstance::setMetatable(const std::string &metatable) {
			push();
			luaL_setmetatable(mState, metatable.c_str());
			lua_pop(mState, 1);
		}

		void IntLuaTableInstance::push(lua_State *state) const
		{
			if (mIndex == LUA_REGISTRYINDEX) {
				lua_pushvalue(state ? state : mState, LUA_REGISTRYINDEX);
			}
			else if (mParent) {
				mParent->push(state ? state : mState);
				lua_rawgeti(state ? state : mState, -1, mIndex);
				lua_replace(state ? state : mState, -2);
			}
			else{
				lua_rawgeti(state ? state : mState, LUA_REGISTRYINDEX, mIndex);
			}
		}

		void StringLuaTableInstance::push(lua_State * state) const
		{
			if (mParent) {
				mParent->push(state ? state : mState);
				lua_getfield(state ? state : mState, -1, mIndex.c_str());
				lua_replace(state ? state : mState, -2);
			}
			else {
				lua_getfield(state ? state : mState, LUA_REGISTRYINDEX, mIndex.c_str());
			}
		}

		void LuaTableInstance::setValue(const std::string & name, const LuaTable & value)
		{
			push();
			value.push();
			lua_setfield(mState, -2, name.c_str());
			lua_pop(mState, 1);
		}

		void LuaTableInstance::setMetatable(const LuaTable & metatable)
		{
			push();
			metatable.push();
			lua_setmetatable(mState, -2);
			lua_pop(mState, 1);
		}

		std::shared_ptr<LuaTableInstance> LuaTableInstance::createTable(lua_State *state, const std::shared_ptr<LuaTableInstance> &ptr)
		{
			push(state ? state : mState);
			lua_newtable(state ? state : mState);
			int index = luaL_ref(state ? state : mState, -2);
			lua_pop(state ? state : mState, 1);
			return std::make_shared<IntLuaTableInstance>(state ? state : mState, index, ptr);
		}

		std::shared_ptr<LuaTableInstance> LuaTableInstance::createTable(const std::string &name, const std::shared_ptr<LuaTableInstance> &ptr)
		{
			push(mState);
			lua_newtable(mState);
			lua_setfield(mState, -2, name.c_str());
			lua_pop(mState, 1);
			return std::make_shared<StringLuaTableInstance>(mState, name, ptr);
		}

		void LuaTable::clear()
		{
			mInstance.reset();
		}

		LuaTable::operator bool() const
		{
			return mInstance.operator bool();
		}

		void LuaTableInstance::setLightUserdata(const std::string & name, void * userdata)
		{
			push();
			lua_pushlightuserdata(mState, userdata);
			lua_setfield(mState, -2, name.c_str());
			lua_pop(mState, 1);
		}

		bool LuaTableInstance::hasFunction(const std::string & name)
		{
			push();
			lua_getfield(mState, -1, name.c_str());
			bool result = lua_isfunction(mState, -1);
			lua_pop(mState, 2);
			return result;
		}

		ArgumentList LuaTableInstance::callMethod(const std::string &name, const ArgumentList &args) {
			int level = lua_gettop(mState);

			push();

			lua_getfield(mState, -1, name.c_str());

			if (!lua_isfunction(mState, -1)) {
				lua_pop(mState, 2);
				throw ScriptingException(Exceptions::unknownMethod(name, "TODO"/*scope->getIdentifier()*/));
			}

			lua_insert(mState, -2);

			args.pushToStack(mState);

			switch (lua_pcall(mState, args.size() + 1, LUA_MULTRET, 0)) {
			case 0:
				break;
			case LUA_ERRRUN: {
				int iType = lua_type(mState, -1);
				const char *msg = "unknown Error";
				if (iType == LUA_TSTRING) {
					msg = lua_tostring(mState, -1);
				}
				else {
					lua_pop(mState, 1);
				}
				throw ScriptingException(std::string("Runtime Error: ") + msg);
			}
			case LUA_ERRMEM:
				throw ScriptingException("Lua Out-Of-Memory!");
			default:
				throw 0;
			}
			int nresults = lua_gettop(mState) - level;

			return ArgumentList(mState, nresults);
		}

		void LuaTable::setValue(const std::string & name, const LuaTable & value)
		{
			mInstance->setValue(name, value);
		}

		void LuaTable::setLightUserdata(const std::string & name, void * userdata)
		{
			mInstance->setLightUserdata(name, userdata);
		}

		void LuaTable::setMetatable(const LuaTable & metatable)
		{
			mInstance->setMetatable(metatable);
		}

		void LuaTable::setMetatable(const std::string & metatable)
		{
			mInstance->setMetatable(metatable);
		}

		bool LuaTable::hasFunction(const std::string & name)
		{
			return mInstance->hasFunction(name);
		}

		ArgumentList LuaTable::callMethod(const std::string & name, const ArgumentList & args)
		{
			return mInstance->callMethod(name, args);
		}

		void LuaTable::push(lua_State * state) const
		{
			mInstance->push(state);
		}

		LuaTable LuaTable::createTable(lua_State * state)
		{
			return mInstance->createTable(state, mInstance);
		}

		LuaTable LuaTable::createTable(const std::string & name)
		{
			return mInstance->createTable(name, mInstance);
		}

		IntLuaTableInstance::IntLuaTableInstance(lua_State * state, int index, const std::shared_ptr<LuaTableInstance>& parent) :
			LuaTableInstance(state, parent),
			mIndex(index)
		{
		}

		StringLuaTableInstance::StringLuaTableInstance(lua_State * state, const std::string &index, const std::shared_ptr<LuaTableInstance>& parent) :
			LuaTableInstance(state, parent),
			mIndex(index)
		{
		}

		LuaTable::LuaTable(const std::shared_ptr<LuaTableInstance> &instance) :
			mInstance(instance) {}

		lua_State * LuaTable::state() const
		{
			return mInstance->state();
		}

		lua_State * LuaTableInstance::state()
		{
			return mState;
		}

	}
}

