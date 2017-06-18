#pragma once

#include "templates.h"
#include "scripting/datatypes/argumentlist.h"



namespace Engine {
	namespace Scripting {

		struct Mapper;

		template <class T>
		struct LuaHelper;

		class INTERFACES_EXPORT APIHelper {
		public:
			static void createMetatables(lua_State *state);

			static void pop(lua_State *state, int n);
			static int stackSize(lua_State *state);
			static int error(lua_State *state, const std::string &msg);
			template <class T>
			static int push(lua_State *state, const T &t) {
				return LuaHelper<T>::push(state, t);
			}
			template <class T>
			static T to(lua_State *state, int index) {
				return LuaHelper<T>::convert(state, index);
			}

		private:
			static int lua_gc(lua_State *state);
			static int lua_toString(lua_State *state);

			static const luaL_Reg sIteratorMetafunctions[];
		};

		template <>
		struct INTERFACES_EXPORT LuaHelper<ValueType> {
			static int push(lua_State *state, const ValueType &v);
			static ValueType convert(lua_State *state, int index);
		};

		template <>
		struct INTERFACES_EXPORT LuaHelper<int(*)(lua_State*)> {
			typedef int(*F)(lua_State*);
			static int push(lua_State *state, int(*f)(lua_State*));
			static F convert(lua_State *state, int index);
		};

		template <>
		struct INTERFACES_EXPORT LuaHelper<ScopeBase*> {
			static int push(lua_State *state, ScopeBase *scope);
			static ScopeBase *convert(lua_State *state, int index);
		};

		template <>
		struct INTERFACES_EXPORT LuaHelper<KeyValueIterator*> {
			static int push(lua_State *state, KeyValueIterator *it);
			static KeyValueIterator *convert(lua_State *state, int index);
		};

		template <>
		struct INTERFACES_EXPORT LuaHelper<Mapper> {
			static int push(lua_State *state, const Mapper &mapper);
		};

	} // namespace Scripting
} // namespace Core
