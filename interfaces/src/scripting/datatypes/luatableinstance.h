#pragma once

namespace Engine
{
	namespace Scripting
	{
		

		class LuaTableInstance
		{
		public:
			LuaTableInstance(lua_State* state, const std::shared_ptr<LuaTableInstance>& parent = {});
			LuaTableInstance(const LuaTableInstance& other) = delete;
			virtual ~LuaTableInstance() = default;

			void setValue(const std::string& name, const ValueType& value);
			ValueType getValue(const std::string& name) const;
			void setLightUserdata(const std::string& name, void* userdata);
			void setMetatable(const LuaTable& metatable);
			void setMetatable(const std::string& metatable);

			bool hasFunction(const std::string& name) const;
			ArgumentList callMethod(const std::string& name, const ArgumentList& args);

			std::shared_ptr<LuaTableInstance> createTable(lua_State* state, const std::shared_ptr<LuaTableInstance>& ptr);
			std::shared_ptr<LuaTableInstance> createTable(const std::string& name, const std::shared_ptr<LuaTableInstance>& ptr);
			std::shared_ptr<LuaTableInstance> registerTable(lua_State* state, int index,
			                                                const std::shared_ptr<LuaTableInstance>& ptr);

			virtual void push(lua_State* state = nullptr) const = 0;

			lua_State* state() const;

		protected:
			std::shared_ptr<LuaTableInstance> mParent;
			lua_State* mState;
		};


	}
}