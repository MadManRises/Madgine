#pragma once

namespace Engine {
	namespace Scripting {


		class LuaTableInstance {
		public:
			LuaTableInstance(lua_State *state, const std::shared_ptr<LuaTableInstance> &parent = {});
			LuaTableInstance(const LuaTableInstance &other) = delete;
			virtual ~LuaTableInstance() = default;

			void setValue(const std::string &name, const LuaTable &value);
			void setLightUserdata(const std::string &name, void *userdata);
			void setMetatable(const LuaTable &metatable);
			void setMetatable(const std::string &metatable);

			bool hasFunction(const std::string &name);
			ArgumentList callMethod(const std::string &name, const ArgumentList &args);

			std::shared_ptr<LuaTableInstance> createTable(lua_State *state, const std::shared_ptr<LuaTableInstance> &ptr);
			std::shared_ptr<LuaTableInstance> createTable(const std::string &name, const std::shared_ptr<LuaTableInstance> &ptr);

			virtual void push(lua_State *state = nullptr) const = 0;

			lua_State *state();

		protected:
			std::shared_ptr<LuaTableInstance> mParent;
			lua_State *mState;
			
		};

		class IntLuaTableInstance : public LuaTableInstance{
		public:
			IntLuaTableInstance(lua_State *state, int index, const std::shared_ptr<LuaTableInstance>& parent = {});			
			virtual ~IntLuaTableInstance();
			virtual void push(lua_State *state = nullptr) const override;
		private:
			int mIndex;
		};

		class StringLuaTableInstance : public LuaTableInstance {
		public:
			StringLuaTableInstance(lua_State *state, const std::string &index, const std::shared_ptr<LuaTableInstance>& parent = {});			
			virtual ~StringLuaTableInstance();
			virtual void push(lua_State *state = nullptr) const override;
		private:
			std::string mIndex;
		};

		class INTERFACES_EXPORT LuaTable {

		public:
			LuaTable();
			

			static LuaTable global(lua_State *state);
			static LuaTable registry(lua_State *state);		

			void setValue(const std::string &name, const LuaTable &value);
			void setLightUserdata(const std::string &name, void *userdata);
			void setMetatable(const LuaTable &metatable);
			void setMetatable(const std::string &metatable);

			bool hasFunction(const std::string &name);
			ArgumentList callMethod(const std::string &name, const ArgumentList &args);

			void push(lua_State *state = nullptr) const;
			void clear();

			operator bool() const;

			LuaTable createTable(lua_State *state = nullptr);
			LuaTable createTable(const std::string &name);

			lua_State *state() const;

		private:
			LuaTable(const std::shared_ptr<LuaTableInstance> &instance);
			

		private:
			std::shared_ptr<LuaTableInstance> mInstance;
		};

		

	}
}