#pragma once

#include "callable_traits.h"

namespace Engine
{
	namespace TupleUnpacker
	{

		template <size_t I, typename Tuple, size_t... S, size_t... T>
		decltype(auto) expand(Tuple&& tuple, std::index_sequence<S...>, std::index_sequence<T...>)
		{
			return std::tuple_cat(
				std::forward_as_tuple(std::get<S>(std::forward<Tuple>(tuple))...),
				std::get<I>(std::forward<Tuple>(tuple)),
				std::forward_as_tuple(std::get<I + 1 + T>(std::forward<Tuple>(tuple))...)
			);
		}

		template <size_t I, typename Tuple>
		decltype(auto) expand(Tuple&& tuple)
		{
			constexpr size_t S = std::tuple_size_v<Tuple>;
			return expand<I>(
				std::forward<Tuple>(tuple),
				std::make_index_sequence<I>(),
				std::make_index_sequence<S - 1 - I>()
			);
		}

		template <class F, size_t... S, class Tuple>
		typename CallableTraits<F>::return_type unpackTuple(F&& f, Tuple&& args, std::index_sequence<S...>)
		{
			return std::invoke(std::forward<F>(f), std::get<S>(std::forward<Tuple>(args))...);
		}

		template <class F, class Tuple>
		typename CallableTraits<F>::return_type invokeTuple(F&& f, Tuple&& args)
		{
			return unpackTuple(std::forward<F>(f), std::forward<Tuple>(args),
				std::make_index_sequence<CallableTraits<F>::argument_count>());
		}

		template <typename F, typename... Args>
		typename CallableTraits<F>::return_type invoke(F&& f, Args&&... args)
		{
			return invokeTuple(std::forward<F>(f), std::forward_as_tuple(std::forward<Args>(args)...));
		}

		template <typename F, typename... Args>
		typename CallableTraits<F>::return_type call(F&& f, Args&&... args)
		{
			return invokeTuple(std::forward<F>(f), expand<sizeof...(args) - 1>(std::forward_as_tuple(std::forward<Args>(args)...)));
		}

	};


}