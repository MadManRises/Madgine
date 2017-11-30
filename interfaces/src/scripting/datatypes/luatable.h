#pragma once

namespace Engine
{
	namespace Scripting
	{
		class INTERFACES_EXPORT LuaThread
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

		class IntLuaTableInstance : public LuaTableInstance
		{
		public:
			IntLuaTableInstance(lua_State* state, int index, const std::shared_ptr<LuaTableInstance>& parent = {});
			virtual ~IntLuaTableInstance();
			void push(lua_State* state = nullptr) const override;
		private:
			int mIndex;
		};

		class StringLuaTableInstance : public LuaTableInstance
		{
		public:
			StringLuaTableInstance(lua_State* state, const std::string& index,
			                       const std::shared_ptr<LuaTableInstance>& parent = {});
			virtual ~StringLuaTableInstance();
			void push(lua_State* state = nullptr) const override;
		private:
			std::string mIndex;
		};

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
