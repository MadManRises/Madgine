#pragma once

#include "templates.h"
#include "scripting/datatypes/argumentlist.h"
#include "scope_cast.h"



namespace Engine {
	namespace Scripting {

	
		struct Mapper {
			Mapper() = delete;
			Mapper(int(*push)(lua_State*)) : mPush(push) {}

			int(*mPush)(lua_State *);

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
			static int push(lua_State *state) {
				return APIHelper::push(state, &FunctionMapperImpl<F, _f, T, R, Ty...>::impl);
			}

		private:

			template <class _, class... _Ty>
			struct Impl {
				template <size_t... I>
				static ValueType call(R(T::*f)(_Ty...), T *my, const ArgumentList &list, const std::index_sequence<I...>) {
					return ValueType((my->*f)(Caster<_Ty>::cast(list.at(I))...));
				}

				template <size_t... I>
				static ValueType call(R(T::*f)(_Ty...) const, const T *my, const ArgumentList &list, const std::index_sequence<I...>) {
					return ValueType((my->*f)(Caster<_Ty>::cast(list.at(I))...));
				}

				template <size_t... I>
				static ValueType callVariadic(R(T::*f)(const ArgumentList &, _Ty...), T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
					return ValueType((my->*f)(variadic, Caster<_Ty>::cast(list.at(I))...));
				}

				template <size_t... I>
				static ValueType callVariadic(R(T::*f)(const ArgumentList &, _Ty...) const, const T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
					return ValueType((my->*f)(variadic, Caster<_Ty>::cast(list.at(I))...));
				}
			};

			template <class... _Ty>
			struct Impl<void, _Ty...> {
				template <size_t... I>
				static ValueType call(void(T::*f)(_Ty...), T *my, const ArgumentList &list, const std::index_sequence<I...>) {
					(my->*f)(Caster<_Ty>::cast(list.at(I))...);
					return ValueType();
				}

				template <size_t... I>
				static ValueType call(void(T::*f)(_Ty...) const, const T *my, const ArgumentList &list, const std::index_sequence<I...>) {
					(my->*f)(Caster<_Ty>::cast(list.at(I))...);
					return ValueType();
				}

				template <size_t... I>
				static ValueType callVariadic(void(T::*f)(const ArgumentList &, _Ty...), T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
					(my->*f)(variadic, Caster<_Ty>::cast(list.at(I))...);
					return ValueType();
				}

				template <size_t... I>
				static ValueType callVariadic(void(T::*f)(const ArgumentList &, _Ty...) const, const T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
					(my->*f)(variadic, Caster<_Ty>::cast(list.at(I))...);
					return ValueType();
				}
			};

			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value, ValueType> wrapped(R(T::*f)(_Ty...), T *t, ArgumentList &&list) {
				return Impl<R, _Ty...>::call(f, t, list, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value, ValueType> wrapped(R(T::*f)(_Ty...) const, const T *t, ArgumentList &&list) {
				return Impl<R, _Ty...>::call(f, t, list, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty>
			static ValueType wrapped(R(T::*f)(const ArgumentList &list, _Ty...), T *t, ArgumentList &&list) {
				auto it = list.begin() + sizeof...(_Ty);
				return Impl<R, _Ty...>::callVariadic(f, t, { list.begin(), it }, { it, list.end() }, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty>
			static ValueType wrapped(R(T::*f)(const ArgumentList &list, _Ty...) const, const T *t, ArgumentList &&list) {
				auto it = list.begin() + sizeof...(_Ty);
				return Impl<R, _Ty...>::callVariadic(f, t, { list.begin(), it }, { it, list.end() }, std::make_index_sequence<sizeof...(_Ty)>());
			}
			
			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value> checkStackSize(lua_State *state, R(T::*f)(_Ty...), int actual){
				if (actual != sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}
			
			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value> checkStackSize(lua_State *state, R(T::*f)(_Ty...) const, int actual){
				if (actual != sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}
			
			template <class... _Ty>
			static void checkStackSize(lua_State *state, R(T::*f)(const ArgumentList &list, _Ty...), int actual){
				if (actual < sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}
						
			template <class... _Ty>
			static void checkStackSize(lua_State *state, R(T::*f)(const ArgumentList &list, _Ty...) const, int actual){
				if (actual < sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			static int impl(lua_State *state) {
				int i = APIHelper::stackSize(state);
				checkStackSize(state, _f, i-1);
					
				ArgumentList args(state, i - 1);
				T *t = dynamic_cast<T*>(APIHelper::to<ScopeBase*>(state, -1));
				APIHelper::pop(state, 1);
				if (!t)
					return APIHelper::error(state, "self has invalid type");
				return APIHelper::push(state, wrapped(_f, t, std::move(args)));
			}

		};

		template <typename F, F f>
		using FunctionMapper = typename MemberFunctionCapture<FunctionMapperImpl, F, f>::type;


		template <class F, F f>
		Mapper make_mapper() {
			return { &FunctionMapper<F, f>::push };
		}



#define MAP(name) \
		{#name, Engine::Scripting::make_mapper<decltype(&C::name), &C::name>()}

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
			static const char *sFile;
			static const char *sName;

			static const char *fix(const char *s) {
				const char *f = strrchr(s, ':');
				return f ? f + 1 : s;
			}
		};

#define API_IMPL(Class, ...) \
	MEMBERS_CAPTURE_DEF(Class, __VA_ARGS__) \
	template<> const char *Engine::Scripting::API<Class>::sFile = __FILE__; \
	template<> const char *Engine::Scripting::API<Class>::sName = fix(#Class); \
	template<> const std::map<std::string, Engine::Scripting::Mapper> Engine::Scripting::API<Class>::sAPI = __hide__::__struct__<Class>::capture()

	} // namespace Scripting
} // namespace Core
