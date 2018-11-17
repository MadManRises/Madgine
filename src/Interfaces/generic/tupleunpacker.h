#pragma once

#include "callable_traits.h"

namespace Engine
{
	struct TupleUnpacker
	{
		template <class R, class T, class... Args, class Tuple = std::tuple<Args...>>
		static R call(T* t, R(T::*f)(Args ...), Tuple&& args)
		{
			return unpack(t, f, std::forward<Tuple>(args),
				std::make_index_sequence<sizeof...(Args)>());
		}

		template <class R, class T, class... Args, size_t... S, class Tuple = std::tuple<Args...>>
		static R unpack(T* t, R(T::*f)(Args ...), Tuple&& args, std::index_sequence<S...>)
		{
			return (t->*f)(std::get<S>(std::forward<Tuple>(args))...);
		}

		template <class R, class T, class... Args, class Tuple = std::tuple<Args...>>
		static R call(const T* t, R(T::*f)(Args ...) const, Tuple&& args)
		{
			return unpack(t, f, std::forward<Tuple>(args),
				std::make_index_sequence<sizeof...(Args)>());
		}

		template <class R, class T, class... Args, size_t... S, class Tuple = std::tuple<Args...>>
		static R unpack(const T* t, R(T::*f)(Args ...) const, Tuple&& args, std::index_sequence<S...>)
		{
			return (t->*f)(std::get<S>(std::forward<Tuple>(args))...);
		}

		template <class R, class... Args, class Tuple = std::tuple<Args...>>
		static R call(R(*f)(Args ...), Tuple&& args)
		{
			return unpack(f, std::forward<Tuple>(args), std::make_index_sequence<sizeof...(Args)>());
		}

		template <class R, class... Args, size_t... S, class Tuple = std::tuple<Args...>>
		static R unpack(R(*f)(Args ...), Tuple&& args, std::index_sequence<S...>)
		{
			return (*f)(std::get<S>(std::forward<Tuple>(args))...);
		}

		template <class F, class Tuple>
		static typename CallableTraits<F>::return_type call(F& f, Tuple&& args)
		{
			return unpack(f, std::forward<Tuple>(args),
				std::make_index_sequence<CallableTraits<F>::argument_count>());
		}

		template <class F, size_t... S, class Tuple>
		static typename CallableTraits<F>::return_type unpack(F& f, Tuple&& args, std::index_sequence<S...>)
		{
			return f(std::get<S>(std::forward<Tuple>(args))...);
		}
	};


}