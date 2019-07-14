#include "../../moduleslib.h"
#include "luatableiterator.h"
#include "luatableinstance.h"
#include "luathread.h"

extern "C"
{
#include "../../lua/lua.h"
}

namespace Engine
{
	namespace Scripting
	{
		LuaTableIterator::LuaTableIterator(const std::shared_ptr<LuaTableInstance>& instance) :
			mInstance(instance)
		{
			std::lock_guard guard(*mInstance->thread());
			lua_State* state = mInstance->thread()->state();
			mInstance->push(state);
			lua_pushnil(state);
			if (lua_next(state, -2) != 0)
			{
				mCurrent.first = lua_tostring(state, -2);
				mCurrent.second = ValueType::fromStack(state, -1);
				lua_pop(state, 2);
			}
			lua_pop(state, 1);
		}

		LuaTableIterator::LuaTableIterator()
		{
		}

		void LuaTableIterator::operator++()
		{
			std::lock_guard guard(*mInstance->thread());
			lua_State* state = mInstance->thread()->state();
			mInstance->push(state);
			lua_pushstring(state, mCurrent.first.c_str());
			if (lua_next(state, -2) != 0)
			{
				mCurrent.first = lua_tostring(state, -2);
				mCurrent.second = ValueType::fromStack(state, -1);
				lua_pop(state, 2);
			}
			else
			{
				mCurrent.first.clear();
				mCurrent.second.clear();
			}
			lua_pop(state, 1);
		}

		const std::pair<std::string, ValueType>& LuaTableIterator::operator*() const
		{
			return mCurrent;
		}


		bool LuaTableIterator::operator!=(const LuaTableIterator& other) const
		{
			return !(*this == other);
		}

		bool LuaTableIterator::operator==(const LuaTableIterator& other) const
		{
			return mCurrent.first == other.mCurrent.first;
		}
	}
}
