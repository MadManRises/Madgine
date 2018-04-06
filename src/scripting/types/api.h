#pragma once

#include "../../generic/templates.h"
#include "../datatypes/argumentlist.h"
#include "scope_cast.h"
#include "mapper.h"
#include "apihelper.h"
#include "../../generic/keyvalue.h"

namespace Engine
{
	KeyValueValueFlags INTERFACES_EXPORT kvFlags(const Scripting::Mapper& mapper);
	KeyValueValueFlags INTERFACES_EXPORT kvFlags(Scripting::Mapper& mapper);

	namespace Scripting
	{
		ValueType INTERFACES_EXPORT toValueType(ScopeBase* ref, const Mapper& mapper);


		template <class T>
		struct Caster
		{
			static decltype(auto) cast(const ValueType& v)
			{
				return v.as<std::remove_const_t<std::remove_reference_t<T>>>();
			}
		};

		template <class T>
		struct Caster<T*>
		{
			static decltype(auto) cast(const ValueType& v)
			{
				T* t = scope_cast<T>(v.as<ScopeBase*>());
				if (!t)
					throw 0;
				return t;
			}
		};

		template <class T, class R>
		struct FunctionMapperBase
		{
			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value
				                        ...>::value, R> callImpl(R (T::*f)(_Ty ...), T* t, const ArgumentList& list)
			{
				return callImpl(f, t, list, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value
				                        ...>::value, R> callImpl(R (T::*f)(_Ty ...) const, T* t, const ArgumentList& list)
			{
				return callImpl(f, t, list, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty>
			static R callImpl(R (T::*f)(const ArgumentList& list, _Ty ...), T* t, const ArgumentList& list)
			{
				auto it = list.begin() + sizeof...(_Ty);
				return callVariadic(f, t, {list.begin(), it}, {it, list.end()}, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class... _Ty, size_t... I>
			static R callImpl(R (T::*f)(_Ty ...), T* my, const ArgumentList& list, const std::index_sequence<I...>)
			{
				return (my ->* f)(Caster<_Ty>::cast(list.at(I))...);
			}

			template <class... _Ty, size_t... I>
			static R callImpl(R (T::*f)(_Ty ...) const, T* my, const ArgumentList& list, const std::index_sequence<I...>)
			{
				return (my ->* f)(Caster<_Ty>::cast(list.at(I))...);
			}

			template <class... _Ty, size_t... I>
			static R callVariadic(R (T::*f)(const ArgumentList&, _Ty ...), T* my, const ArgumentList& list,
			                      const ArgumentList& variadic, const std::index_sequence<I...>)
			{
				return (my ->* f)(variadic, Caster<_Ty>::cast(list.at(I))...);
			}

			template <class... _Ty, size_t... I>
			static R callVariadic(R (T::*f)(const ArgumentList&, _Ty ...) const, const T* my, const ArgumentList& list,
			                      const ArgumentList& variadic, const std::index_sequence<I...>)
			{
				return (my ->* f)(variadic, Caster<_Ty>::cast(list.at(I))...);
			}


			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value
				...>::value> checkStackSize(lua_State* state, R (T::*f)(_Ty ...), int actual)
			{
				if (actual != sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value
				...>::value> checkStackSize(lua_State* state, R (T::*f)(_Ty ...) const, int actual)
			{
				if (actual != sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static void checkStackSize(lua_State* state, R (T::*f)(const ArgumentList& list, _Ty ...), int actual)
			{
				if (actual < sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static void checkStackSize(lua_State* state, R (T::*f)(const ArgumentList& list, _Ty ...) const, int actual)
			{
				if (actual < sizeof...(_Ty))
					APIHelper::error(state, Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			/*static int impl(lua_State *state) {
			int i = APIHelper::stackSize(state);
			checkStackSize(state, _f, i - 1);

			ArgumentList args(state, i - 1);
			T *t = dynamic_cast<T*>(APIHelper::to<ScopeBase*>(state, -1));
			APIHelper::pop(state, 1);
			if (!t)
			return APIHelper::error(state, "self has invalid type");
			return APIHelper::push(state, call(t, std::move(args)));
			}*/
		};

		template <class F, F _f, class T, class R, class... Ty>
		class FunctionMapperImpl
		{
		public:

			typedef T type;

			static ValueType call(T* t, const ArgumentList& args)
			{
				return ValueType(FunctionMapperBase<T, R>::callImpl(_f, t, args));
			}
		};

		template <class F, F _f, class T, class... Ty>
		class FunctionMapperImpl<F, _f, T, void, Ty...>
		{
		public:

			typedef T type;

			static ValueType call(T* t, const ArgumentList& args)
			{
				FunctionMapperBase<T, void>::callImpl(_f, t, args);
				return ValueType();
			}
		};

		template <typename F, F f>
		using FunctionMapper = typename MemberFunctionCapture<FunctionMapperImpl, F, f>::type;

		template <class F, F f>
		Mapper make_function_mapper()
		{
			return Mapper(&Mapper::map_f<typename FunctionMapper<F, f>::type, &FunctionMapper<F, f>::call>);
		}

		template <class G, G g>
		Mapper make_mapper_readonly()
		{
			struct GetterWrapper
			{
				static ValueType get(ScopeBase* ref)
				{
					using T = typename FunctionMapper<G, g>::type;
					T* t = dynamic_cast<T*>(ref);
					return FunctionMapper<G, g>::call(t, {});
				}
			};
			return {&GetterWrapper::get};
		}

		template <class G, G g, class S, S s>
		Mapper make_mapper()
		{
			struct GetterWrapper
			{
				static ValueType get(ScopeBase* ref)
				{
					using T = typename FunctionMapper<G, g>::type;
					T* t = dynamic_cast<T*>(ref);
					return FunctionMapper<G, g>::call(t, {});
				}
			};
			struct SetterWrapper
			{
				static void set(ScopeBase* ref, const ValueType& v)
				{
					using T = typename FunctionMapper<S, s>::type;
					T* t = dynamic_cast<T*>(ref);
					FunctionMapper<S, s>::call(t, {v});
				}
			};
			return {&GetterWrapper::get, &SetterWrapper::set};
		}


#define MAP_F(name) \
		std::pair<const std::string, Engine::Scripting::Mapper>{#name, Engine::Scripting::make_function_mapper<decltype(&C::name), &C::name>()}

#define MAP_RO(name, getter) \
		std::pair<const std::string, Engine::Scripting::Mapper>{#name, Engine::Scripting::make_mapper_readonly<decltype(&C::getter), &C::getter>()}

#define MAP(name, getter, setter) \
		std::pair<const std::string, Engine::Scripting::Mapper>{#name, Engine::Scripting::make_mapper<decltype(&C::getter), &C::getter, decltype(&C::setter), &C::setter>()}

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

		template<class T>
		struct TEMPLATE_EXPORT API
		{
		public:
			static const char* const file();

			static const char* const name();

			static const std::map<std::string, Mapper> &api();
			

		private:


			static const char* fix(const char* s)
			{
				const char* f = strrchr(s, ':');
				return f ? f + 1 : s;
			}

		};



#define API_IMPL(Class, ...) \
	MEMBERS_CAPTURE_DEF(Class, __VA_ARGS__) \
	template<> TEMPLATE_INSTANCE const char * const Engine::Scripting::API<Class>::file() {static const char *s = __FILE__; return s; } \
	template<> TEMPLATE_INSTANCE const char * const Engine::Scripting::API<Class>::name() {static const char *s = fix(#Class); return s; } \
	template<> TEMPLATE_INSTANCE const std::map<std::string, Engine::Scripting::Mapper> &Engine::Scripting::API<Class>::api(){ \
		static std::map<std::string, Engine::Scripting::Mapper> m = __hide__::__struct__<Class>::capture(); return m;}\
	template struct TEMPLATE_INSTANCE Engine::Scripting::API<Class>

	} // namespace Scripting
} // namespace Core
