#pragma once

namespace Engine
{
	namespace Scripting
	{
		class LuaThread
		{
		public:
			LuaThread(lua_State* state) :
				mState(state)
			{
			}

			bool operator==(const LuaThread& other) const
			{
				return other.mState == mState;
			}

			operator lua_State *() const
			{
				return mState;
			}

		private:
			lua_State* mState;
		};

	}
}