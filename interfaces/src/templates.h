#pragma once


namespace Engine {

	template <class...>
	using void_t = void;

	template<class...>
	struct type_pack;

	template<size_t, class T>
	struct select_type;

	template<size_t I, class Head, class... Tail>
	struct select_type<I, type_pack<Head, Tail...>>
		: select_type<I - 1, type_pack<Tail...>> {};

	template<class Head, class... Tail>
	struct select_type<0, type_pack<Head, Tail...>> {
		typedef Head type;
	};

	template<typename T, size_t n, size_t... Is>
	struct type_pack_selector
		: type_pack_selector<T, n - 1, n - 1, Is...> {};

	template <typename T, size_t... Is>
	struct type_pack_selector<T, 0, Is...> {
		using type = type_pack<typename select_type<Is, T>::type...>;
	};

	template<size_t n, class... _Ty>
	using type_pack_n = typename type_pack_selector<type_pack<_Ty...>, n>::type;

	template<bool...>
	struct bool_pack;

	template<bool... values>
	struct all_of
		: std::is_same<bool_pack<values..., true>, bool_pack<true, values...>> {};



	template <class V, class T>
	struct variant_contains;

	template <class T>
	struct variant_contains<std::variant<>, T> {
		const constexpr static bool value = false;
	};

	template <class... V, class T>
	struct variant_contains<std::variant<T, V...>, T> {
		const constexpr static bool value = true;
	};

	template <class U, class... V, class T>
	struct variant_contains<std::variant<U, V...>, T> {
		const constexpr static bool value = variant_contains<std::variant<V...>, T>::value;
	};

	template <class V, class T>
	struct variant_index;

	template <class... V, class T>
	struct variant_index<std::variant<T, V...>, T> {
		const constexpr static size_t value = 0;
	};

	template <class U, class... V, class T>
	struct variant_index<std::variant<U, V...>, T> {
		const constexpr static size_t value = 1 + variant_index<std::variant<V...>, T>::value;
	};

	struct TupleSerializer {

		template <class... Args>
		static void readTuple(std::tuple<Args...> &tuple, Serialize::SerializeInStream &in) {
			readTuple(tuple, in, std::make_index_sequence<sizeof...(Args)>());
		}

		template <class Arg, class... Args, size_t... S>
		static void readTuple(std::tuple<Arg, Args...> &tuple, Serialize::SerializeInStream &in, std::index_sequence<S...>) {
			using expander = int[];
			(void)expander {
				(void(in >> std::get<S>(tuple)), 0)...
			};
		}

		static void readTuple(std::tuple<> &tuple, Serialize::SerializeInStream &in, std::index_sequence<>) {
		}

		template <class... Args>
		static void writeTuple(const std::tuple<Args...> &tuple, Serialize::SerializeOutStream &out) {
			writeTuple(tuple, out, std::make_index_sequence<sizeof...(Args)>());
		}

		template <class Arg, class... Args, size_t... S>
		static void writeTuple(const std::tuple<Arg, Args...> &tuple, Serialize::SerializeOutStream &out, std::index_sequence<S...>) {
			using expander = int[];
			(void)expander {
				(void(out << std::get<S>(tuple)), 0)...
			};
		}

		static void writeTuple(const std::tuple<> &tuple, Serialize::SerializeOutStream &out, std::index_sequence<>) {
		}

	};



	template <class R, class T, class... _Ty>
	constexpr size_t argumentsCount(R(T::*)(_Ty...) const) {
		return sizeof...(_Ty);
	}

	template <class R, class T, class... _Ty>
	constexpr size_t argumentsCount(R(T::*)(_Ty...)) {
		return sizeof...(_Ty);
	}



	template <class T, class R, class... _Ty>
	struct MemberFunctionTypeCapture {
	public:
		template <template <typename F, F, class, class, class...> class C, typename F, F f, class... Args>
		using instance = C<F, f, Args..., T, R, _Ty...>;
	};

	template <class T, class R, class... _Ty>
	MemberFunctionTypeCapture<T, R, _Ty...> memberFunctionTypeDeducer(R(T::*f)(_Ty...));

	template <class T, class R, class... _Ty>
	MemberFunctionTypeCapture<const T, R, _Ty...> memberFunctionTypeDeducer(R(T::*f)(_Ty...) const);

	template <typename F>
	using MemberFunctionHelper = decltype(memberFunctionTypeDeducer(std::declval<F>()));

	template <template <typename F, F, class, class, class...> class C, class F, F f, class... Args>
	struct MemberFunctionCapture {
		typedef typename MemberFunctionHelper<F>::template instance<C, F, f, Args...> type;
	};



	template <class R, class... _Ty>
	struct CallableTypeCapture {
	public:
		template <template <typename, class, class...> class C, typename F, class... Args>
		using instance = C<F, Args..., R, _Ty...>;

		typedef R Return;
	};

	template <class T, class R, class... _Ty>
	CallableTypeCapture<R, _Ty...> CallableTypeDeducer(R(T::*f)(_Ty...));

	template <class T, class R, class... _Ty>
	CallableTypeCapture<R, _Ty...> CallableTypeDeducer(R(T::*f)(_Ty...) const);

	template <class R, class... _Ty>
	CallableTypeCapture<R, _Ty...> CallableTypeDeducer(R(*f)(_Ty...));

	template <class F, class R = decltype(CallableTypeDeducer(&F::operator()))>
	R CallableTypeDeducer(const F &);

	/*template <template <typename F, F, class, class...> class C, auto f, class... Args>
	using MemberFunctionCapture = typename decltype(memberFunctionTypeDeducer(f))::template instance<C, decltype(f), f, Args...>;
	*/

	template <typename F>
	using CallableHelper = decltype(CallableTypeDeducer(std::declval<F>()));

	template <template <typename F, class, class...> class C, class F, class... Args>
	struct CallableDeduce {
		typedef typename CallableHelper<F>::template instance<C, F, Args...> type;
	};



	template <class... _Ty>
	struct TupleUnpacker {

		template <class R, class T, class... Args, class Tuple = std::tuple<Args...>>
		static R call(T *t, R(T::*f)(_Ty..., Args...), _Ty... _ty, Tuple&& args) {
			return unpack(t, f, std::forward<_Ty>(_ty)..., std::forward<Tuple>(args), std::make_index_sequence<sizeof...(Args)>());
		}

		template <class R, class T, class... Args, size_t... S, class Tuple = std::tuple<Args...>>
		static R unpack(T *t, R(T::*f)(_Ty..., Args...), _Ty... _ty, Tuple&& args, std::index_sequence<S...>) {
			return (t->*f)(std::forward<_Ty>(_ty)..., std::get<S>(std::forward<Tuple>(args))...);
		}

		template <class R, class T, class... Args, class Tuple = std::tuple<Args...>>
		static R call(const T *t, R(T::*f)(_Ty..., Args...) const, _Ty... _ty, Tuple&& args) {
			return unpack(t, f, std::forward<_Ty>(_ty)..., std::forward<Tuple>(args), std::make_index_sequence<sizeof...(Args)>());
		}

		template <class R, class T, class... Args, size_t... S, class Tuple = std::tuple<Args...>>
		static R unpack(const T *t, R(T::*f)(_Ty..., Args...) const, _Ty... _ty, Tuple&& args, std::index_sequence<S...>) {
			return (t->*f)(std::forward<_Ty>(_ty)..., std::get<S>(std::forward<Tuple>(args))...);
		}

		template <class R, class... Args, class Tuple = std::tuple<Args...>>
		static R call(R(*f)(_Ty..., Args...), _Ty... _ty, Tuple&& args) {
			return unpack(f, std::forward<_Ty>(_ty)..., std::forward<Tuple>(args), std::make_index_sequence<sizeof...(Args)>());
		}

		template <class R, class... Args, size_t... S, class Tuple = std::tuple<Args...>>
		static R unpack(R(*f)(_Ty..., Args...), _Ty... _ty, Tuple&& args, std::index_sequence<S...>) {
			return (*f)(std::forward<_Ty>(_ty)..., std::get<S>(std::forward<Tuple>(args))...);
		}

		template <class F, class Tuple>
		static typename CallableHelper<F>::Return call(F &f, _Ty... _ty, Tuple&& args) {
			return unpack(f, std::forward<_Ty>(_ty)..., std::forward<Tuple>(args), std::make_index_sequence<argumentsCount(&F::operator())>());
		}

		template <class F, size_t... S, class Tuple>
		static typename CallableHelper<F>::Return unpack(F &f, _Ty... _ty, Tuple&& args, std::index_sequence<S...>) {
			return f(std::forward<_Ty>(_ty)..., std::get<S>(std::forward<Tuple>(args))...);
		}

	};

	class CopyType {

	};

	class NonCopyType {
	protected:
		NonCopyType() = default;
		NonCopyType(const NonCopyType &) = delete;
		void operator=(const NonCopyType &) = delete;
		NonCopyType(NonCopyType &&) = default;
		NonCopyType &operator=(NonCopyType &&) = default;
	};

	template <class, class T>
	struct _custom_is_copy_constructible : std::false_type {
	};

	template <class T>
	struct _custom_is_copy_constructible<void_t<decltype(T(std::declval<const T &>()))>, T> : std::true_type {
	};

	template <class T>
	struct custom_is_copy_constructible {
		static const constexpr bool value = _custom_is_copy_constructible<void_t<>, T>::value;
	};

	template <class U, class V>
	struct custom_is_copy_constructible<std::pair<U, V>> {
		static constexpr const bool value = custom_is_copy_constructible<U>::value && custom_is_copy_constructible<V>::value;
	};

	template <class T>
	using CopyTraits = std::conditional_t<custom_is_copy_constructible<T>::value, CopyType, NonCopyType>;


	template <class T>
	class is_iterable {
	private:
		template<typename C> static std::true_type Test(void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>*);
		template<typename> static std::false_type Test(...);

	public:
		static bool const constexpr value = decltype(Test<T>(0))::value;
	};

}



