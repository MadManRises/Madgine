#pragma once

namespace Engine
{


	template <typename F, typename R, typename T, typename... _Ty>
	struct CallableType
	{

		typedef F type;
		typedef R return_type;
		typedef T class_type;
		typedef std::tuple<_Ty...> argument_types;
		static constexpr size_t argument_count = sizeof...(_Ty);

		template <template <typename, typename, typename...> typename C, typename... Args>
		struct instance {
			using type = C<Args..., R, T, _Ty...>;
		};
	};

	namespace __generic__impl__ {
		template <typename R, typename T, typename... _Ty>
		CallableType<R(T::*)(_Ty ...), R, T, _Ty...> callableTypeDeducer(R(T::*f)(_Ty ...));

		template <typename R, typename T, typename... _Ty>
		CallableType<R(T::*)(_Ty ...) const, R, const T, _Ty...> callableTypeDeducer(R(T::*f)(_Ty ...) const);

		template <typename R, typename... _Ty>
		CallableType<R(*)(_Ty ...), R, void, _Ty...> callableTypeDeducer(R(*f)(_Ty ...));

		template <typename F, typename R = decltype(callableTypeDeducer(&F::operator()))>
		R callableTypeDeducer(const F&);
	}

	template <typename F>
	using CallableTraits = decltype(__generic__impl__::callableTypeDeducer(std::declval<F>()));

	template <typename T>
	struct Wrap {
		using type = T;
	};

	template <template <auto, typename, typename, typename...> typename C, auto f>
	struct Partial {
		template <typename Arg1, typename Arg2, typename... Args>
		struct apply {
			using type = C<f, Arg1, Arg2, Args...>;
		};
	};	

	template <auto f>
	struct Callable {
		using traits = CallableTraits<decltype(f)>;
		static constexpr typename traits::type value = f;


		template <template <auto, typename, typename, typename...> typename C, typename... Args>
		struct instance {
			using type = typename traits::template instance<Partial<C, f>::template apply, Args...>::type;
		};
	};

	template <template <auto, typename, typename, typename...> typename C, auto f, typename... Args>
	using MemberFunctionCapture = typename Callable<f>::template instance<C, Args...>::type;
}