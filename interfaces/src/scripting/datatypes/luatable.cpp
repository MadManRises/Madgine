#include "interfaceslib.h"
#include "luatable.h"
#include "argumentlist.h"
#include "luatableiterator.h"
#include "intluatableinstance.h"

extern "C"
{
#include <lua.h>                                /* Always include this when calling Lua */
}

namespace Engine
{
	namespace Scripting
	{
		LuaTable::LuaTable()
		{
		}

		LuaTable LuaTable::global(lua_State* state)
		{
			return std::static_pointer_cast<LuaTableInstance>(std::make_shared<IntLuaTableInstance>(state, LUA_RIDX_GLOBALS));
		}

		LuaTable LuaTable::registry(lua_State* state)
		{
			return std::static_pointer_cast<LuaTableInstance>(std::make_shared<IntLuaTableInstance>(state, LUA_REGISTRYINDEX));
		}

		void LuaTable::clear()
		{
			mInstance.reset();
		}

		LuaTable::operator bool() const
		{
			return mInstance.operator bool();
		}

		void LuaTable::setValue(const std::string& name, const ValueType& value)
		{
			mInstance->setValue(name, value);
		}

		ValueType LuaTable::getValue(const std::string& name) const
		{
			return mInstance ? mInstance->getValue(name) : ValueType();
		}

		void LuaTable::setLightUserdata(const std::string& name, void* userdata)
		{
			mInstance->setLightUserdata(name, userdata);
		}

		void LuaTable::setMetatable(const LuaTable& metatable)
		{
			mInstance->setMetatable(metatable);
		}

		void LuaTable::setMetatable(const std::string& metatable)
		{
			mInstance->setMetatable(metatable);
		}

		bool LuaTable::hasFunction(const std::string& name) const
		{
			return mInstance ? mInstance->hasFunction(name) : false;
		}

		ArgumentList LuaTable::callMethod(const std::string& name, const ArgumentList& args)
		{
			return mInstance->callMethod(name, args);
		}

		void LuaTable::push(lua_State* state) const
		{
			mInstance->push(state);
		}

		LuaTable LuaTable::createTable(lua_State* state)
		{
			return mInstance->createTable(state, mInstance);
		}

		LuaTable LuaTable::createTable(const std::string& name)
		{
			return mInstance->createTable(name, mInstance);
		}

		LuaTable LuaTable::registerTable(lua_State* state, int index)
		{
			return mInstance->registerTable(state, index, mInstance);
		}

		LuaTable::LuaTable(const std::shared_ptr<LuaTableInstance>& instance) :
			mInstance(instance)
		{
		}

		lua_State* LuaTable::state() const
		{
			return mInstance->state();
		}

		LuaTable::iterator LuaTable::begin() const
		{
			return iterator(mInstance);
		}

		LuaTable::iterator LuaTable::end() const
		{
			return iterator();
		}
	}
}
