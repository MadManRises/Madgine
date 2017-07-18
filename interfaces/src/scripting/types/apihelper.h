#pragma once

#include "templates.h"
#include "scripting/datatypes/argumentlist.h"



namespace Engine {
	namespace Scripting {

		struct Mapper;

		template <class T>
		struct LuaHelper;

		template <class T>
		struct LuaRefHelper;

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
			static int push(lua_State *state, ScopeBase *ref, const T &t) {
				return LuaRefHelper<T>::push(state, ref, t);
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
			static int push(lua_State *state, F f);
			static F convert(lua_State *state, int index);
		};

		template <>
		struct INTERFACES_EXPORT LuaHelper<KeyValueIterator*> {
			static int push(lua_State *state, KeyValueIterator *it);
			static KeyValueIterator *convert(lua_State *state, int index);
		};

		template <class T>
		struct Unmapper {
			static int push(lua_State *state, ScopeBase *ref, const T &t) {
				return LuaHelper<T>::push(state, t);
			}
		};


		template <class T>
		struct LuaRefHelper {
			static int push(lua_State *state, ScopeBase *ref, const T &t) {
				return Unmapper<T>::push(state, ref, t);
			}
		};

		template <class T>
		struct LuaHelper {
			static int push(lua_State *state, const T &v) {
				return LuaHelper<ValueType>::push(state, ValueType(v));
			}
			static T convert(lua_State *state, int index) {
				return LuaHelper<ValueType>::convert(state, index).as<T>();
			}
		};

	} // namespace Scripting
} // namespace Core
