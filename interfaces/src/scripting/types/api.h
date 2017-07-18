#pragma once

#include "templates.h"
#include "scripting/datatypes/argumentlist.h"
#include "scope_cast.h"



namespace Engine {
	namespace Scripting {


		struct Mapper {
			Mapper() = delete;
			Mapper(int(*push)(lua_State *, ScopeBase *), std::pair<bool, ValueType>(*toValueType)(ScopeBase *)) : mPush(push), mToValueType(toValueType) {}

			int (*mPush)(lua_State *, ScopeBase *);
			std::pair<bool, ValueType>(*mToValueType)(ScopeBase *);

		};

		template <>
		struct Unmapper<Mapper> {
			static int push(lua_State *state, ScopeBase *ref, const Mapper &m) {
				return m.mPush(state, ref);
			}
		};

		

		template <class T>
		struct Caster {
			static auto cast(const ValueType &v) {
				return v.as<std::remove_const_t<std::remove_reference_t<T>>>();
			}
		};

		template <class T>
		struct Caster<T*> {
			static auto cast(const ValueType &v) {
				T *t = scope_cast<T>(v.asScope());
				if (!t)
					throw 0;
				return t;
			}
		};


		template <class F, F _f, class T, class R, class... Ty>
		class FunctionMapperImpl {
		public:

			typedef T T;

			static int push(lua_State *state, ScopeBase *ref) {
				return APIHelper::push(state, &FunctionMapperImpl<F, _f, T, R, Ty...>::impl);
			}

			static auto call(T *t, const ArgumentList &args) {
				return FixReturn<R>::call(t, args);
			}


		private:
			template <class _R>
			struct FixReturn {
				static _R call(T *t, const ArgumentList &args) {
					return callImpl(_f, t, args);
				}
			};

			template <>
			struct FixReturn<void> {
				static ValueType call(T *t, const ArgumentList &args) {
					callImpl(_f, t, args);
					return ValueType();
				}
			};

			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value, R> callImpl(R(T::*f)(_Ty...), T *t, const ArgumentList &list) {
				return callImpl(f, t, list, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value, R> callImpl(R(T::*f)(_Ty...) const, T *t, const ArgumentList &list) {
				return callImpl(f, t, list, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty>
			static R callImpl(R(T::*f)(const ArgumentList &list, _Ty...), T *t, const ArgumentList &list) {
				auto it = list.begin() + sizeof...(_Ty);
				return callVariadic(f, t, { list.begin(), it }, { it, list.end() }, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty, size_t... I>
			static R callImpl(R(T::*f)(_Ty...), T *my, const ArgumentList &list, const std::index_sequence<I...>) {
				return (my->*f)(Caster<_Ty>::cast(list.at(I))...);
			}

			template <class... _Ty, size_t... I>
			static R callImpl(R(T::*f)(_Ty...) const, T *my, const ArgumentList &list, const std::index_sequence<I...>) {
				return (my->*f)(Caster<_Ty>::cast(list.at(I))...);
			}

			template <class... _Ty, size_t... I>
			static R callVariadic(R(T::*f)(const ArgumentList &, _Ty...), T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
				return (my->*f)(variadic, Caster<_Ty>::cast(list.at(I))...);
			}

			template <class... _Ty, size_t... I>
			static R callVariadic(R(T::*f)(const ArgumentList &, _Ty...) const, const T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
				return (my->*f)(variadic, Caster<_Ty>::cast(list.at(I))...);
			}


			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value> checkStackSize(lua_State *state, R(T::*f)(_Ty...), int actual) {
				if (actual != sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value> checkStackSize(lua_State *state, R(T::*f)(_Ty...) const, int actual) {
				if (actual != sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static void checkStackSize(lua_State *state, R(T::*f)(const ArgumentList &list, _Ty...), int actual) {
				if (actual < sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static void checkStackSize(lua_State *state, R(T::*f)(const ArgumentList &list, _Ty...) const, int actual) {
				if (actual < sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			static int impl(lua_State *state) {
				int i = APIHelper::stackSize(state);
				checkStackSize(state, _f, i - 1);

				ArgumentList args(state, i - 1);
				T *t = dynamic_cast<T*>(APIHelper::to<ScopeBase*>(state, -1));
				APIHelper::pop(state, 1);
				if (!t)
					return APIHelper::error(state, "self has invalid type");
				return APIHelper::push(state, call(t, std::move(args)));
			}

		};

		template <typename F, F f>
		using FunctionMapper = typename MemberFunctionCapture<FunctionMapperImpl, F, f>::type;


		template <class F, F f>
		Mapper make_function_mapper() {
			return { &FunctionMapper<F, f>::push, nullptr };
		}

		template <class G, G g, class S = void*, S s = nullptr>
		Mapper make_mapper() {
			struct GetterWrapper {
				static int push(lua_State *state, ScopeBase *ref) {
					using T = typename FunctionMapper<G, g>::T;
					T *t = dynamic_cast<T*>(ref);
					return APIHelper::push(state, FunctionMapper<G, g>::call(t, {}));
				}
				static std::pair<bool, ValueType> valueType(ScopeBase *ref) {
					using T = typename FunctionMapper<G, g>::T;
					T *t = dynamic_cast<T*>(ref);
					return toValueType(ref, FunctionMapper<G, g>::call(t, {}));
				}
			};
			return { &GetterWrapper::push, &GetterWrapper::valueType };
		}



#define MAP_F(name) \
		std::pair<const std::string, Engine::Scripting::Mapper>{#name, Engine::Scripting::make_function_mapper<decltype(&C::name), &C::name>()}

#define MAP_RO(name, getter) \
		std::pair<const std::string, Engine::Scripting::Mapper>{#name, Engine::Scripting::make_mapper<decltype(&C::getter), &C::getter>()}

#define MEMBERS_CAPTURE_DEF(Class, ...) \
	namespace __hide__{ \
		template <class C> \
		struct __struct__; \
		\
		template <> \
		struct __struct__<Class>{ \
			static std::map<std::string, Engine::Scripting::Mapper> capture() { \
				using C = Class; \
				return {{__VA_ARGS__}};	\
			} \
		};\
	}

		template <class T>
		struct API {
			static const std::map<std::string, Mapper> sAPI;
			static const char * const sFile;
			static const char * const sName;

			static const char *fix(const char *s) {
				const char *f = strrchr(s, ':');
				return f ? f + 1 : s;
			}
		};

#define API_IMPL(Class, ...) \
	MEMBERS_CAPTURE_DEF(Class, __VA_ARGS__) \
	template<> const char * const Engine::Scripting::API<Class>::sFile = __FILE__; \
	template<> const char * const Engine::Scripting::API<Class>::sName = fix(#Class); \
	template<> const std::map<std::string, Engine::Scripting::Mapper> Engine::Scripting::API<Class>::sAPI = __hide__::__struct__<Class>::capture()

	} // namespace Scripting
} // namespace Core
