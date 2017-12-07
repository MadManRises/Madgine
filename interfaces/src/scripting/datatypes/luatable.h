#pragma once

namespace Engine
{
	namespace Scripting
	{

		class LuaTableInstance;

		class INTERFACES_EXPORT LuaTable
		{
		public:
			LuaTable();


			static LuaTable global(lua_State* state);
			static LuaTable registry(lua_State* state);

			void setValue(const std::string& name, const ValueType& value);
			ValueType getValue(const std::string& name) const;
			void setLightUserdata(const std::string& name, void* userdata);
			void setMetatable(const LuaTable& metatable);
			void setMetatable(const std::string& metatable);

			bool hasFunction(const std::string& name) const;
			ArgumentList callMethod(const std::string& name, const ArgumentList& args);

			void push(lua_State* state = nullptr) const;
			void clear();

			operator bool() const;

			LuaTable createTable(lua_State* state = nullptr);
			LuaTable createTable(const std::string& name);
			LuaTable registerTable(lua_State* state, int index);

			lua_State* state() const;

			typedef LuaTableIterator iterator;

			iterator begin() const;
			iterator end() const;

		private:
			LuaTable(const std::shared_ptr<LuaTableInstance>& instance);


		private:
			std::shared_ptr<LuaTableInstance> mInstance;
		};
	}
}
