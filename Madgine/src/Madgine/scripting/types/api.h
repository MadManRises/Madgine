#pragma once

#include "../../generic/callable_traits.h"
#include "../datatypes/argumentlist.h"
#include "scope_cast.h"
#include "mapper.h"
#include "apihelper.h"
#include "../../generic/valuetype.h"

namespace Engine
{
	KeyValueValueFlags INTERFACES_EXPORT kvFlags(const Scripting::Mapper& mapper);
	KeyValueValueFlags INTERFACES_EXPORT kvFlags(Scripting::Mapper& mapper);

	namespace Scripting
	{
		ValueType INTERFACES_EXPORT toValueType(ScopeBase* ref, const Mapper& mapper);
		void INTERFACES_EXPORT fromValueType(ScopeBase* ref, const Mapper& mapper, const ValueType &value);


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
				return scope_cast<T>(v.as<ScopeBase*>());
			}
		};

		template <class R, class T>
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
					APIHelper::error(state, Database::Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value
				...>::value> checkStackSize(lua_State* state, R (T::*f)(_Ty ...) const, int actual)
			{
				if (actual != sizeof...(_Ty))
					APIHelper::error(state, Database::Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static void checkStackSize(lua_State* state, R (T::*f)(const ArgumentList& list, _Ty ...), int actual)
			{
				if (actual < sizeof...(_Ty))
					APIHelper::error(state, Database::Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

			template <class... _Ty>
			static void checkStackSize(lua_State* state, R (T::*f)(const ArgumentList& list, _Ty ...) const, int actual)
			{
				if (actual < sizeof...(_Ty))
					APIHelper::error(state, Database::Exceptions::argumentCountMismatch(sizeof...(_Ty), actual));
			}

		};

		template <auto _f, class R, class T, class... Ty>
		class FunctionMapperImpl
		{
		public:

			typedef T type;

			static ValueType call(T* t, const ArgumentList& args)
			{
				return ValueType(FunctionMapperBase<R, T>::callImpl(_f, t, args));
			}
		};

		template <auto _f, class T, class... Ty>
		class FunctionMapperImpl<_f, void, T, Ty...>
		{
		public:

			typedef T type;

			static ValueType call(T* t, const ArgumentList& args)
			{
				FunctionMapperBase<void, T>::callImpl(_f, t, args);
				return ValueType();
			}
		};

		template <auto f>
		using FunctionMapper = typename MemberFunctionCapture<FunctionMapperImpl, f>::type;

		template <auto f>
		Mapper make_function_mapper()
		{
			return Mapper(&Mapper::map_f<typename FunctionMapper<f>::type, &FunctionMapper<f>::call>);
		}

		template <auto g>
		Mapper make_mapper_readonly()
		{
			struct GetterWrapper
			{
				static ValueType get(ScopeBase* ref)
				{
					using T = typename FunctionMapper<g>::type;
					T* t = dynamic_cast<T*>(ref);
					return FunctionMapper<g>::call(t, {});
				}
			};
			return {&GetterWrapper::get};
		}

		template <auto g, auto s>
		Mapper make_mapper()
		{
			struct GetterWrapper
			{
				static ValueType get(ScopeBase* ref)
				{
					using T = typename FunctionMapper<g>::type;
					T* t = dynamic_cast<T*>(ref);
					return FunctionMapper<g>::call(t, {});
				}
			};
			struct SetterWrapper
			{
				static void set(ScopeBase* ref, const ValueType& v)
				{
					using T = typename FunctionMapper<s>::type;
					T* t = dynamic_cast<T*>(ref);
					FunctionMapper<s>::call(t, {v});
				}
			};
			return {&GetterWrapper::get, &SetterWrapper::set};
		}


#define MAP_F(name) \
		std::pair<const std::string, Engine::Scripting::Mapper>{#name, Engine::Scripting::make_function_mapper<&std::decay_t<decltype(*this)>::name>()}

#define MAP_RO(name, getter) \
		std::pair<const std::string, Engine::Scripting::Mapper>{#name, Engine::Scripting::make_mapper_readonly<&std::decay_t<decltype(*this)>::getter>()}

#define MAP(name, getter, setter) \
		std::pair<const std::string, Engine::Scripting::Mapper>{#name, Engine::Scripting::make_mapper<&std::decay_t<decltype(*this)>::getter, &std::decay_t<decltype(*this)>::setter>()}

	} // namespace Scripting
} // namespace Core
