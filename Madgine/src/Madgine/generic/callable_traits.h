#pragma once

namespace Engine
{

	template <class R, class T, class... _Ty>
	constexpr size_t argumentsCount(R(T::*)(_Ty ...) const)
	{
		return sizeof...(_Ty);
	}

	template <class R, class T, class... _Ty>
	constexpr size_t argumentsCount(R(T::*)(_Ty ...))
	{
		return sizeof...(_Ty);
	}



	namespace __generic__impl__
	{

		template <class T, class R, class... _Ty>
		struct MemberFunctionTypeCapture
		{
		public:
			template <template <auto, class, class, class...> class C, auto f, class... Args>
			using instance = C<f, Args..., T, R, _Ty...>;
		};

		template <class T, class R, class... _Ty>
		MemberFunctionTypeCapture<T, R, _Ty...> memberFunctionTypeDeducer(R(T::*f)(_Ty ...));

		template <class T, class R, class... _Ty>
		MemberFunctionTypeCapture<const T, R, _Ty...> memberFunctionTypeDeducer(R(T::*f)(_Ty ...) const);

		template <auto f>
		using MemberFunctionHelper = decltype(memberFunctionTypeDeducer(f));
	}



	template <template <auto, class, class, class...> class C, auto f, class... Args>
	struct MemberFunctionCapture
	{
		typedef typename __generic__impl__::MemberFunctionHelper<f>::template instance<C, f, Args...> type;
	};


	template <class R, class... _Ty>
	struct CallableTypeCapture
	{
	public:
		template <template <typename, class, class...> class C, typename F, class... Args>
		using instance = C<F, Args..., R, _Ty...>;

		typedef R Return;
	};

	namespace __generic__impl__ {
		template <class T, class R, class... _Ty>
		CallableTypeCapture<R, _Ty...> callableTypeDeducer(R(T::*f)(_Ty ...));

		template <class T, class R, class... _Ty>
		CallableTypeCapture<R, _Ty...> callableTypeDeducer(R(T::*f)(_Ty ...) const);

		template <class R, class... _Ty>
		CallableTypeCapture<R, _Ty...> callableTypeDeducer(R(*f)(_Ty ...));

		template <class F, class R = decltype(callableTypeDeducer(&F::operator()))>
		R callableTypeDeducer(const F&);


		template <typename F>
		using CallableHelper = decltype(callableTypeDeducer(std::declval<F>()));
	}

	template <template <typename F, class, class...> class C, class F, class... Args>
	struct CallableDeduce
	{
		typedef typename __generic__impl__::CallableHelper<F>::template instance<C, F, Args...> type;
	};
}